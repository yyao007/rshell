#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <string>
using namespace std;

int GetRedirectCmd(string &, vector<string> &, vector<string> &);
int SplitInOutCmd(string &, vector<string> &, unsigned, long unsigned);

int main() {
    string cmd;
    vector<string> outRe;
    vector<string> inRe;
    cout << "input string: ";
    getline(cin, cmd);

    if (-1 == GetRedirectCmd(cmd, outRe, inRe)) {
        cout << "syntax error using redirect operator" << endl;
    }

    return 0;
}

int GetRedirectCmd(string &cmdLine, vector<string> &outRe, vector<string> &inRe) {
    outRe.clear();
    inRe.clear();
    long unsigned len = 0;
    long unsigned pos = 0;
    string temp;

    pos = cmdLine.find(">");
    while(pos != string::npos) {
        // find ">" redirect flag
        if (pos + 1 < cmdLine.size() && cmdLine.at(pos + 1) != '>') {
            if (-1 == SplitInOutCmd(cmdLine, outRe, 1, pos)) {
                return -1; // invalid use of '>'
            }
            pos = cmdLine.find(">", pos - 1);
        }
        // find ">>" redirect flag
        else if (pos + 2 < cmdLine.size() && cmdLine.at(pos + 2) != '>') {
            if (-1 == SplitInOutCmd(cmdLine, outRe, 2, pos)) {
                return -1; // invalid use of ">>"
            }
            pos = cmdLine.find(">", pos - 1);
        }
        // return -1 for other flags
        else {
            return -1;
        }
    }

    pos = cmdLine.find("<");
    while (pos != string::npos) {
        // find "<" redirect flag
        if (pos + 1 < cmdLine.size() && cmdLine.at(pos + 1) != '<') {
            if (-1 == SplitInOutCmd(cmdLine, inRe, 1, pos)) {
                return -1;
            }
            pos = cmdLine.find("<", pos - 1);
        }
        // find "<<<" redirect flag
        else if (pos + 3 < cmdLine.size() && cmdLine.at(pos + 2) == '<' && cmdLine.at(pos + 3) != '<') {
            if (-1 == SplitInOutCmd(cmdLine, inRe, 3, pos)) {
                return -1;
            }
            pos = cmdLine.find("<", pos - 1);
        }
        else {
            return -1;
        }
    }
    return 0;
}

int SplitInOutCmd(string &cmdLine, vector<string> &cmd, unsigned num, long unsigned pos) {
    while (pos + num < cmdLine.size() && cmdLine.at(pos + num) == ' ') {
        cmdLine.erase(pos + num, 1);
    }
    unsigned len = num;
    if (pos + len >= cmdLine.size()) {
        return -1;
    }
    bool isGoing = (cmdLine.at(pos + len) != ' ') &&
    (cmdLine.at(pos + len) != '>') && (cmdLine.at(pos + len) != '<');

    while (isGoing) {
        ++len;
        isGoing = false;
        if (pos + len < cmdLine.size()) {
            isGoing = (cmdLine.at(pos + len) != ' ') &&
            (cmdLine.at(pos + len) != '>') && (cmdLine.at(pos + len) != '<');
        }
    }
    // if there's no argument after the redirect character, return -1 for error
    if(len == num) {
        return -1;
    }
    cmd.push_back(cmdLine.substr(pos, len));
    cmdLine.erase(pos, len);

    return 0;
}


