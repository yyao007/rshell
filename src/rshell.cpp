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
void split(char *arr[], char str[], const char *delim);

// get command block between connectors
int getCommand(char **, const string&, int&, int);

int main(int argc, char *argv[]) {
    char origStr[cap]; // store the user input line
    char *save_1, *save_2; // save value using in strtok_r
    int i = 0;

    // always in my rshell
    while (1) {
        cout << "$ ";
        char *cmd[cap]; // declare a 2-D 1000*1000 array
        // pre-allocate cmd
        for(i = 0; i < cap; ++i) {
            cmd[i] = new char[cap];
        }

        bool isExecuted = true;
        char *effectStr;
        char errcmd[cap];
        string userStr; // change a cstring into string
        int flag = 4;
        int index = 0;
        int status;

        memset(origStr, 0, cap); // initialize origStr to an empty string
        cin.getline(origStr, cap); // get the user input line

        if (origStr[0] == '\0') {
            continue;
        }

        effectStr = strtok(origStr, "#"); // any command after the "#" is comment
        userStr = effectStr;

        while (flag != 0 && isExecuted) {
            if ( (flag = getCommand(cmd, userStr, index, flag)) == -1 ) {
                cout << "Error: Cannot start with connectors \"|| && ;\"." << endl;
                break;
            }

            if ( (strcmp(cmd[0], "exit") == 0) && cmd[1] == NULL) {
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

int getCommand(char *cmd[], const string& str, int& index, int iFlag) {
    string substring;
    const int size = 3;
    int flag[size] = {0};
    char cstr[cap];
    int min;
    int mFlag;
    int i;
    int begin;
    int leng;

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
    int pos[size] = {(str.size() - 1), (str.size() - 1), (str.size() - 1)};

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


    // if the string begins with connectors, an error message -1 will return
    if (min == index) {
        return -1;
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

