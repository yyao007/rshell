#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int getCommand(char *cmd[], const string& str, int& index, int iFlag) {
    string substring;
    const int size = 3;
    int flag[size] = {0};
    char cstr[1000];
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


int main() {
    char *cmd[1000];
    string str;
    int index;
    int flag;
    int i;

    for (i = 0; i < 1000; ++i) {
        cmd[i] = new char[1000];
    }

    cout << "string: ";
    getline(cin, str);

    flag = 4;
    index = 0;

    while (flag != 0) {

        if ((flag = getCommand(cmd, str, index, flag)) == -1) {
            cout << "Error" << endl;
        }
        int pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(1);
        }
        else if (pid == 0) {
            if (execvp(cmd[0], cmd) == -1) {
                perror(cmd[0]);

            }
            exit(1);
        }

        else {
            int *status = 0;
            int res = wait(&status);
            if (res == -1) {
                perror("wait");
            }
        }
    }

/*    i = 0;
    while (flag != 0) {
            cout << "Cannot start with connectors \"|| && ;\"." << endl;
        }

        cout << cmd[i] << endl;
        ++i;
    }
*/
    return 0;
}








