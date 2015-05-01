#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <string>

using namespace std;

const int cap = 1000; // the capacity of the 2-D array

// split a line by the delim and store to a 2-D array
//void split(char *arr[], char str[], const char *delim);
// get command block between connectors
int getCommand(char **, string&, int&, int);
// check if the string has syntax error
int strValid(const string&);

int main(int argc, char *argv[]) {
    char origStr[cap]; // store the user input line
    int i = 0;
    char hostname[100];
    char *username;

    if (-1 == gethostname(hostname, 100)) {
        perror("gethostname()");
        exit(1);
    }
    if (NULL == (username = getlogin())) {
        perror("getlogin()");
        exit(1);
    }

    // always in my rshell
    while (1) {
    mylabel:
        cout << username << '@' << hostname << " $ ";
        char *cmd[cap]; // declare a 2-D 1000*1000 array
        // pre-allocate cmd
        for(i = 0; i < cap; ++i) {
            cmd[i] = new char[cap];
        }

        bool isExecuted = true; // initialize to true in each loop
        char *effectStr; // store the string before the "#"
        char *spaceStr; // check if the string only has spaces
        char errcmd[cap]; // store a copy of command name
        char origCpy[cap]; // store a copy of original string
        string userStr; // change a cstring into string
        string tempStr; // store the additional string
        int flag = 4; // 4 indicates first call
        int index = 0; // initialize to 0 in order to look through the whole string
        int status; // store the status of child exit
        int errStr; // store the return value of strValid

        memset(origStr, 0, cap); // initialize origStr to an empty string
        cin.getline(origStr, cap); // get the user input line
        // copy from origStr to origCpy to avoid modifying origStr
        strcpy(origCpy, origStr);
        // if the user just hit enter or enter several spaces, go to the next loop
        spaceStr = strtok(origCpy, " ");
        if (origStr[0] == '\0' || spaceStr == NULL || spaceStr[0] == '#' ) {
            continue;
        }

        effectStr = strtok(origStr, "#"); // any command after the "#" is comment
        userStr = effectStr;

        // check the string at first
        errStr = strValid(userStr);
        if (errStr == -1) {
            cout << "syntax error using connectors \"|| && ;\"" << endl;
            continue;
        }
        else if (errStr == -2) {
            cout << "Error: Cannot start with connectors \"|| && ;\"." << endl;
            continue;
        }

        // if the string ended with any connectors, ask the user to input other commands
        while (errStr == 0) {
            cout << "<command>: ";
            memset(origStr, 0, cap); // initialize origStr to an empty string
            effectStr = new char;
            cin.getline(origStr, cap);

            // check if the new string contains the "#" character
            if (origStr[0] == '#' || origStr[0] == '\0') {
                continue;
            }
            effectStr = strtok(origStr, "#");
            tempStr = effectStr;

            userStr = userStr + tempStr;

            // check the new string in each loop
            errStr = strValid(userStr);
            if (errStr == -1) {
                cout << "syntax error using connectors \"|| && ;\"" << endl;
                goto mylabel;
            }
        }

        // run execvp() in a while loop
        while (flag != 0 && isExecuted) {
            flag = getCommand(cmd, userStr, index, flag);

            // if the command is "exit", exit the rshell
            if ( (strcmp(cmd[0], "exit") == 0) ) {
                return 0;
            }

            int pid = fork(); // create a child process to call execvp()

            // fork’s return value for an error is -1
            if (pid == -1) {
                perror("fork");
                exit(1); //there was an error with fork so exit the program and go back and fix it
            }

            // this is in child process
            else if (pid == 0) {
                // run execvp in child process in order not to exit the whole program
                if (execvp(cmd[0], cmd) == -1) {
                    strcpy(errcmd, cmd[0]);
                    perror(errcmd);
                    exit(EXIT_FAILURE);
                }

                exit(EXIT_SUCCESS); // kill the child process when it's done
            }

            // this is in parent process
            else {
                // wait for child process to finish executing
                if (wait(&status) == -1) {
                    perror("wait()");
                    exit(1);
                }
                // if the command fails and the connector is &&, do not execute the next command
                if (WEXITSTATUS(status) == EXIT_FAILURE && flag == 2) {
                    isExecuted = false;
                }
                // if the command is executed and the connector is ||, do not execute the next command
                else if (WEXITSTATUS(status) == EXIT_SUCCESS && flag == 1) {
                    isExecuted = false;
                }
                // execute the next command if it's not the situations above
                else {
                    isExecuted = true;
                }
            }
        }
    }

    return 0;
}

/*
void split(char *arr[], char str[], const char *delim) {
    int i = 0; // initialize i to 0 before the loop

    // split the whole line by delim
    arr[i] = strtok(str, delim);
    while (arr[i] != NULL) {
        ++i;
        arr[i] = strtok(NULL, delim);
    }

    return;
}
*/

int getCommand(char *cmd[], string& str, int& index, int iFlag) {
    string substring;
    const int size = 3;
    int flag[size] = {0};
    char cstr[cap];
    long unsigned int min;
    int mFlag;
    int i;
    long unsigned int begin;
    long unsigned int leng;

    // get the begin point of the substring according to the iFlag
    if (iFlag == 4) {  // 4 represents the first call
        begin = index;
    }
    else if (iFlag == 3) {
        begin = index + 1;
    }
    else {
        begin = index + 2;
    }

    // initialize pos[] to str's length
    long unsigned int pos[size] = {(str.size() - 1), (str.size() - 1), (str.size() - 1)};

    // find each connector, 1 represents "||", 2 represents "&&",
    // 3 represents ";", 0 represents no connector
    if ((pos[0] = str.find("||", begin)) != string::npos) {
        flag[0] = 1;
    }
    if ((pos[1] = str.find("&&", begin)) != string::npos) {
        flag[1] = 2;
    }
    if ((pos[2] = str.find(";", begin)) != string::npos) {
        flag[2] = 3;
    }

    // find the first occurrence of any connector
    min = str.size() - 1;
    mFlag = 0;
    for (i = 0; i < size; ++i) {
        if (min > pos[i] && pos[i] != string::npos) {
            min = pos[i];
            mFlag = flag[i];
        }
    }

    // get the length of substring
    if (mFlag == 0) {
        leng = min - begin + 1;
    }
    else {
        leng = min - begin;
    }
    // get the first command block
    substring = str.substr(begin, leng);

    // do toking
    strcpy(cstr, substring.c_str());

    i = 0;
    cmd[i] = strtok(cstr, " ");

    while (cmd[i] != NULL) {
        ++i;
        cmd[i] = strtok(NULL, " ");
    }

    index = min; // update the index to the first occurrence of any connector
    return mFlag;
}

int strValid(const string& str) {
    long unsigned int i;
    long unsigned int pos;
    int flag = 1; // initialize flag to 1 indicates valid string
    string temp = str;

    pos = temp.find_first_of("|&");
    while (pos != string::npos) {
        // if string is ended with "| &", return -1
        if (pos == temp.size() - 1) {
            return -1;
        }
        // if string only has 1 of "| &" or has different combinations like "|&", return -1
        else if (temp.at(pos + 1) != temp.at(pos)) {
            return -1;
        }
        pos = temp.find_first_of("|&", pos + 2);
    }

    // remove all the spaces in the string and store to temp
    for (i = 0; i < temp.size(); ++i) {
        if (temp.at(i) == ' ') {
            temp.erase(i, 1);
            --i;
        }
    }
    // if the character after any connector is either of "| & ;",
    // despite white spaces, then it's a syntax error
    pos = temp.find_first_of("|&");
    while (pos != string::npos) {
        if (temp.size() > 2) {
            // if string is ended with "||" or "&&", jump out of the loop and return 0
            if (pos == temp.size() - 2) {
                flag = 0;
                break;
            }
            // if string has more than 2 of "| & ;", return -1
            else if (temp.at(pos + 2) == '|' || temp.at(pos + 2) == '&' || temp.at(pos + 2) == ';') {
                return -1;
            }
        }
        // if the string begins with connectors, an error message -2 will return
        if (pos == 0) {
            return -2;
        }
//        // if string is ended with "| &", return -1
//        else if (pos == temp.size() - 1) {
//            return -1;
//        }
//        // if string only has one of "| &" or has different combinations "like |&", return -1
//        else if (temp.at(pos + 1) != temp.at(pos)) {
//            return -1;
//        }
        pos = temp.find_first_of("|&", pos + 2);
    }

    pos = temp.find(";");
    while (pos != string::npos) {
        if (temp.size() > 1) {
            // if string is ended with ";", jump out of the loop and return 0
            if (pos == temp.size() - 1) {
                flag = 0;
                break;
            }
            // if string has more than 1 of ";" or something like ";|", return -1
            else if (temp.at(pos + 1) == '|' || temp.at(pos + 1) == '&' || temp.at(pos + 1) == ';') {
                return -1;
            }
        }
        // if the string begins with connectors, an error message -2 will return
        if (pos == 0) {
            return -2;
        }
        pos = temp.find(";", pos + 1);
    }

    return flag;
}


