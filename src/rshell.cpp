#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <signal.h>

using namespace std;

int status; // store the status of child exit
int childpid = -1;
vector<int> stopchild;
bool isKilled = false;
char *currPath;
const int cap = 512; // the capacity of the 2-D array
const int PIPE_READ = 0;
const int PIPE_WRITE = 1;

// signal handler
struct sigaction inter;
struct sigaction stop;
struct sigaction ignore;
void interrupthdl(int, siginfo_t *, void *);
void stophdl(int, siginfo_t *, void *);

// split a line by the delim and store to a 2-D array
int split(char **arr, char *str, const char *delim);
// get command block between connectors
int getCommand(const string &, int &, int, string &);
// check if the string has syntax error
int strValid(const string &);
int GetRedirectCmd(string &, vector<string> &);
int SplitInOutCmd(string &, vector<string> &, unsigned, long unsigned);
void Redirect(vector<string> &);
bool isRedirect(const string &);
bool isEmpty(const string&);
bool isPiping(const string &);
void Piping(string &, bool &, int, int savestdin = -1);
int ChangeDir(const char *);
void SimplifyPath(string &);
int Foreground(int);

int main(int argc, char *argv[]) {
    char origStr[cap]; // store the user input line
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
    // reserve signals
    inter.sa_sigaction = interrupthdl;
    inter.sa_flags = SA_SIGINFO;
    stop.sa_sigaction = stophdl;
    stop.sa_flags = SA_SIGINFO;

    if (-1 == sigaction(SIGINT, &inter, NULL)) {
        perror("sigaction()");
        exit(1);
    }
    if (-1 == sigaction(SIGTSTP, &stop, NULL)) {
        perror("sigaction()");
        exit(1);
    }

    char path[cap] = "/home/csmajs/yyao007/cs100/rshell/bin:";
    strcat(path, getenv("PATH"));
    setenv("PATH", path, 1);

    // always in my rshell
    while (1) {
    mylabel:
        childpid = -1;
        isKilled = false;
        if ((currPath = getenv("PWD")) == NULL) {
            perror("getenv()");
            exit(1);
        }
        string simplePath = currPath;
        SimplifyPath(simplePath);
        cout << username << '@' << hostname << ":" << simplePath << " $ ";
        bool isExecuted = true; // initialize to true in each loop
        char *effectStr; // store the string before the "#"
        char *spaceStr; // check if the string only has spaces
        char origCpy[cap]; // store a copy of original string
        string userStr; // change a cstring into string
        string tempStr; // store the additional string
        int flag = 4; // 4 indicates first call
        int index = 0; // initialize to 0 in order to look through the whole string
        int errStr; // store the return value of strValid
        vector<string> ReFile;

        cin.clear();
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
            char *efftmp;
            cin.getline(origStr, cap);

            // check if the new string contains the "#" character
            if (origStr[0] == '#' || origStr[0] == '\0') {
                continue;
            }
            efftmp = strtok(origStr, "#");
            tempStr = efftmp;

            userStr = userStr + tempStr;

            // check the new string in each loop
            errStr = strValid(userStr);
            if (errStr == -1) {
                cout << "syntax error using connectors \"|| && ;\"" << endl;
                goto mylabel;
            }
        }

        while (flag != 0) {
            string cmdStr;
            flag = getCommand(userStr, index, flag, cmdStr);
            if (-1 == GetRedirectCmd(cmdStr, ReFile)) {
                cout << "syntax error using redirect operators \"> <\"" << endl;
                goto mylabel;
            }
        }

        flag = 4;
        index = 0;
        // run execvp() in a while loop
        while (flag != 0 && isExecuted) {
            ReFile.clear();
            string cmdStr;
            string storeStr;

            flag = getCommand(userStr, index, flag, cmdStr);
            Piping(cmdStr, isExecuted, flag);
            if (isKilled) {
                break;
            }
        }
    }
    return 0;
}


int split(char **arr, char *str, const char *delim) {
    int i = 0;
    int count = 0;

    char *temp = strtok(str, delim);
    while (temp != NULL) {
        arr[i] = new char[100];
        strcpy(arr[i], temp);
        ++count;
        ++i;
        temp = strtok(NULL, delim);
   }

    return count;
}

int getCommand(const string &str, int &index, int iFlag, string &substring) {
    char *cmd[cap] = {0};
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
    int count = split(cmd, cstr, " ");
    // if the command is "exit", exit the rshell
    if ((strcmp(cmd[0], "exit") == 0) ) {
        for (int i = 0; i < count; ++i) {
            if (cmd[i] != 0) {
                delete[] cmd[i];
                cmd[i] = 0;
            }
        }
        exit(0);
    }
    for (int i = 0; i < count; ++i) {
        if (cmd[i] != 0) {
            delete[] cmd[i];
            cmd[i] = 0;
        }
    }

    index = min; // update the index to the first occurrence of any connector
    return mFlag;
}

int strValid(const string &str) {
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
        // if string has different combinations like "|&", return -1
        else if (temp.at(pos + 1) != temp.at(pos)) {
            if (temp.at(pos) == '&') {
                return -1;
            }
            else if (temp.at(pos + 1) == '&') {
                return -1;
            }
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
            if (pos == temp.size() - 1) {
                return -1;
            }
            // if string is ended with "||" or "&&", jump out of the loop and return 0
            else if (pos == temp.size() - 2 && temp.at(pos) == temp.at(pos + 1)) {
                flag = 0;
                break;
            }
            // if string has more than 2 of "| & ;", return -1
            else if ((pos < temp.size() - 2) && (temp.at(pos) == temp.at(pos + 1)) &&
                (temp.at(pos + 2) == '|' || temp.at(pos + 2) == '&' || temp.at(pos + 2) == ';')) {
                return -1;
            }
        }
        // if the string begins with connectors, an error message -2 will return
        if (pos == 0) {
            return -2;
        }
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

int GetRedirectCmd(string &cmdLine, vector<string> &ReFile) {
    ReFile.clear();
    long unsigned pos = 0;
    string temp;

    pos = cmdLine.find_first_of("<>");
    while(pos != string::npos) {
        // find ">" redirect flag
        if (cmdLine.at(pos) == '>') {
            if (pos + 1 < cmdLine.size() && cmdLine.at(pos + 1) != '>') {
                if (-1 == SplitInOutCmd(cmdLine, ReFile, 1, pos)) {
                    return -1; // invalid use of '>'
                }
                pos = cmdLine.find_first_of("<>", pos);
            }
            // find ">>" redirect flag
            else if (pos + 2 < cmdLine.size() && cmdLine.at(pos + 2) != '>') {
                if (-1 == SplitInOutCmd(cmdLine, ReFile, 2, pos)) {
                    return -1; // invalid use of ">>"
                }
                pos = cmdLine.find_first_of("<>", pos);
            }
            // return -1 for other flags
            else {
                return -1;
            }
        }
        // find "<" redirect flag
        else if (cmdLine.at(pos) == '<') {
            if (pos + 1 < cmdLine.size() && cmdLine.at(pos + 1) != '<') {
                if (-1 == SplitInOutCmd(cmdLine, ReFile, 1, pos)) {
                    return -1;
                }
                pos = cmdLine.find_first_of("<>", pos);
            }
            // find "<<<" redirect flag
            else if (pos + 3 < cmdLine.size() && cmdLine.at(pos + 2) == '<' && cmdLine.at(pos + 3) != '<') {
                if (-1 == SplitInOutCmd(cmdLine, ReFile, 3, pos)) {
                    return -1;
                }
                pos = cmdLine.find_first_of("<>", pos);
            }
            else {
                return -1;
            }
        }
    }

    return 0;
}

int SplitInOutCmd(string &cmdLine, vector<string> &cmd, unsigned num, long unsigned pos) {
    while (pos + num < cmdLine.size() && cmdLine.at(pos + num) == ' ') {
        cmdLine.erase(pos + num, 1);
    }

    long unsigned subpos = 0; // store the length of the number before ">"
    unsigned len = num; // store the number of characters of ioredirection flag
    // find if there is a number before ">"
    if (cmdLine.at(pos) == '>') {
        for (int i = pos - 1; i >= 0 && isdigit(cmdLine.at(i)); --i) {
            ++subpos;
            if (i > 0) {
                if (cmdLine.at(i - 1) == ' ') {
                    break;
                }
                else if (!isdigit(cmdLine.at(i - 1))) {
                    subpos = 0;
                    break;
                }
            }
        }
    }
    // return -1 when a redirect flag is in the end
    if (pos + len >= cmdLine.size()) {
        return -1;
    }
    // consider "<<<" flag separately
    if (num == 3) {
        long unsigned quotePos = pos;
        int even = 0;
        long unsigned redirectPos = cmdLine.find_first_of("<>", pos + 3);
        quotePos = cmdLine.find("\"", quotePos);

        // find double quotes and check if they are in pairs
        if (quotePos != string::npos && (quotePos < redirectPos)) {
            while (quotePos != string::npos) {
                ++even;
                len = quotePos - pos + 1;
                if (even % 2 == 0) {
                    // break loop if quotePos is the end of the string
                    if (quotePos == cmdLine.size() - 1) {
                        break;
                    }
                    // set len to the end of the string if there is no space, double quotes
                    // and redirect operators after the right side of the double quotes
                    else if (cmdLine.find_first_of("\"<> ", quotePos + 1) == string::npos) {
                        len = cmdLine.size() - pos;
                        break;
                    }
                    else {
                        // find the first occurrence of either of the four characters
                        quotePos = cmdLine.find_first_of("\"<> ", quotePos + 1);
                        // set len to quotePos - pos + 1 if the space occurs first
                        if (cmdLine.at(quotePos) == ' ') {
                            len = quotePos - pos + 1;
                            break;
                        }
                        // set len to quotePos - pos if the "<" or ">" occurs first
                        else if (cmdLine.at(quotePos) != '\"'){
                            len = quotePos - pos;
                            break;
                        }
                        ++even;
                    }
                }
                quotePos = cmdLine.find("\"", quotePos + 1);
            }
            // if double quotes are in pairs, push back to the vector and return 0
            if (even % 2 == 0) {
                cmd.push_back(cmdLine.substr(pos - subpos, len + subpos));
                cmdLine.erase(pos - subpos, len + subpos);
                return 0;
            }
        }
    }

    len = num;
    // if a command like 2>  2>, will be invalid
    if (isdigit(cmdLine.at(pos + len))) {
        return -1;
    }
    bool isGoing = (cmdLine.at(pos + len) != ' ') &&
    (cmdLine.at(pos + len) != '>') && (cmdLine.at(pos + len) != '<');
    // find the length of the redirect file name
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
    cmd.push_back(cmdLine.substr(pos - subpos, len + subpos));
    cmdLine.erase(pos - subpos, len + subpos);

    return 0;
}

void Redirect(vector<string> &ReFile) {
    char wholeName[cap];
    // do ioredirection in the original order
    for (unsigned i = 0; i < ReFile.size(); ++i) {
        int fd;
        int len = 0;
        string strnum;
        int num;

        strcpy(wholeName, ReFile.at(i).c_str());
        for (len = 0; isdigit(ReFile.at(i).at(len)); ++len) {
            strnum += ReFile.at(i).at(len);
        }
        // "2>" and "2>>" case
        if (len > 0) {
            num = atoi(strnum.c_str());
            if (ReFile.at(i).at(1 + len) == '>') {
                char *fileName = wholeName + len + 2;
                if (-1 == (fd = open(fileName, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR))) {
                    perror(fileName);
                    exit(EXIT_FAILURE);
                }
                if (-1 == dup2(fd, num)) {
                    perror("dup2()");
                    exit(EXIT_FAILURE);
                }
            }
            else {
                char *fileName = wholeName + len + 1;
                if (-1 == (fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR))) {
                    perror(fileName);
                    exit(EXIT_FAILURE);
                }
                if (-1 == dup2(fd, num)) {
                    perror("dup2()");
                    exit(EXIT_FAILURE);
                }
            }
        }
        // ">>" case
        else if (ReFile.at(i).at(1) == '>') {
            char *fileName = wholeName + 2;
            if (-1 == (fd = open(fileName, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR))) {
                perror(fileName);
                exit(EXIT_FAILURE);
            }
            if (-1 == dup2(fd, 1)) {
                perror("dup2()");
                exit(EXIT_FAILURE);
            }
        }
        // ">" case
        else if (ReFile.at(i).at(0) == '>'){
            char *fileName = wholeName + 1;
            if (-1 == (fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR))) {
                perror(fileName);
                exit(EXIT_FAILURE);
            }
            if (-1 == dup2(fd, 1)) {
                perror("dup2()");
                exit(EXIT_FAILURE);
            }
        }
        // "<<<" case
        else if (ReFile.at(i).size() >= 3 && ReFile.at(i).at(2) == '<') {
            char *fileName = wholeName + 3;
            string str = fileName;
            int even = 0; // check if the string are in a set of double quotes
            for (unsigned j = 0; j < str.size(); ++j) {
                if (str.at(j) == '\"') {
                    str.erase(j, 1);
                    --j;
                    ++even;
                }
            }
            if (even % 2 == 1) {
                cerr << "need a string inside of a set of double quotes" << endl;
                exit(EXIT_FAILURE);
            }
            str += '\n';

            int fd1[2];
            if (-1 == pipe(fd1)) {
                perror("pipe()");
                exit(1);
            }
            // write to the pipe
            if (-1 == write(fd1[PIPE_WRITE], str.c_str(), str.size() + 1)) {
                perror("write()");
                exit(1);
            }
            // read from the pipe
            if (-1 == dup2(fd1[PIPE_READ], 0)) {
                perror("dup2()");
                exit(1);
            }
            if (-1 == close(fd1[PIPE_WRITE])) {
                perror("close()");
                exit(1);
            }
        }
        // "<" case
        else {
            char *fileName = wholeName + 1;
            if (-1 == (fd = open(fileName, O_RDONLY, S_IRUSR))) {
                perror(fileName);
                exit(EXIT_FAILURE);
            }
            if (-1 == dup2(fd, 0)) {
                perror("dup2()");
                exit(EXIT_FAILURE);
            }
        }
    }
    return;
}

bool isRedirect(const string &cmdLine) {
    long unsigned pos = cmdLine.find_first_of("<>");
    return (pos != string::npos);
}

bool isPiping(const string &cmdLine) {
    long unsigned pos = cmdLine.find("|");
    return (pos != string::npos);
}

bool isEmpty(const string& str) {
    for (unsigned i = 0; i < str.size(); ++i) {
        if (str.at(i) != ' ') {
            return false;
        }
    }
    return true;
}

void Piping(string &cmdLine, bool &isExecuted, int flag, int savestdin) {
    long unsigned pos = 0;
    int count = 0;
    char *cmd[cap] = {0};
    char cstr[cap];
    string cmdStr;
    vector<string> ReFile;
    bool isChdir = false;
    bool chdirFail = false;
    bool isfg = false;
    bool fgfail = false;
    childpid = -1;

    if (!isPiping(cmdLine)) {
        if (isRedirect(cmdLine)) {
            GetRedirectCmd(cmdLine, ReFile);
        }
        if (!isEmpty(cmdLine)) {
            char cmdStrCpy[cap];
            strcpy(cmdStrCpy, cmdLine.c_str());
            count = split(cmd, cmdStrCpy, " ");
        }

        // check if has the cd command
        if (strcmp(cmd[0], "cd") == 0) {
            isChdir = true;
            if (-1 == ChangeDir(cmd[1])) {
                chdirFail = true;
            }
        }

        // check if the command is fg
        else if (strcmp(cmd[0], "fg") == 0) {
            isfg = true;
            if (cmd[1] != 0) {
                cout << "fg cannot handle any flags" << endl;
                fgfail = true;
            }
            else if (-1 == Foreground(0)) {
                cout << "fg: current: no such job" << endl;
                fgfail = true;
            }
        }
        // check if the command is bg
        else if (strcmp(cmd[0], "bg") == 0) {
            isfg = true;
            if (cmd[1] != 0) {
                cout << "bg cannot handle any flags" << endl;
                fgfail = true;
            }
            else if (-1 == Foreground(1)) {
                cout << "bg: current: no such job" << endl;
                fgfail = true;
            }
        }

        int pid = fork();

        if (pid == -1) {
            perror("fork()");
            exit(1);
        }

        else if (pid == 0) {
            // ignore SIGINT in the child process in order to ignore ctrl C
            // from parent when a child is in the background
            ignore.sa_handler = SIG_IGN;
            if (-1 == sigaction(SIGINT, &ignore, NULL)) {
                perror("sigaction()");
                exit(1);
            }
            if (-1 == sigaction(SIGTSTP, &ignore, NULL)) {
                perror("sigaction()");
                exit(1);
            }
            Redirect(ReFile);
            if (chdirFail || fgfail) {
                exit(EXIT_FAILURE);
            }
            else if (isChdir || isfg) {
                exit(EXIT_SUCCESS);
            }

            // run execvp in child process in order not to exit the whole program
            else if (execvp(cmd[0], cmd) == -1) {
                char errcmd[cap];
                strcpy(errcmd, cmd[0]);
                perror(errcmd);
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS); // kill the child process when it's done
        }

        // this is in parent process
        else {
            childpid = pid;
            errno = 0;
            // wait for child process to finish executing
            if (-1 == waitpid(-1, &status, WUNTRACED) && errno == EINTR) {
                if (-1 == waitpid(-1, &status, WUNTRACED)) {
                    perror("wait() 730");
                    exit(1);
                }
            }
            if (errno != 0 && errno != EINTR) {
                perror("waitpid()");
                exit(1);
            }
            // if the command fails and the connector is &&, do not execute the next command
            if (WEXITSTATUS(status) != 0 && flag == 2) {
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
            for (int i = 0; i < count; ++i) {
                if (cmd[i] != 0) {
                    delete[] cmd[i];
                    cmd[i] = 0;
                }
            }
        }
    }

    else {
        if ((pos = cmdLine.find("|")) != string::npos) {
            cmdStr = cmdLine.substr(0, pos);
            cmdLine.erase(0, pos + 1);
            if (isRedirect(cmdStr)) {
                GetRedirectCmd(cmdStr, ReFile);
                if (!isEmpty(cmdStr)) {
                    strcpy(cstr, cmdStr.c_str());
                    count = split(cmd, cstr, " ");
                }
            }
            else {
                strcpy(cstr, cmdStr.c_str());
                count = split(cmd, cstr, " ");
            }

            int fd[2];
            if (-1 == pipe(fd)) {
                perror("pipe()");
                exit(1);
            }
            int pid = fork();
            if (pid == -1) {
                perror("fork()");
                exit(1);
            }
            // write to the pipe in child process
            else if(pid == 0) {

                if (-1 == dup2(fd[PIPE_WRITE], 1)) {
                    perror("dup2()");
                    exit(1);
                }
                if (-1 == close(fd[PIPE_READ])) {
                    perror("close()");
                    exit(1);
                }
                Redirect(ReFile);
                // check if has the cd command
                if (strcmp(cmd[0], "cd") == 0) {
                    if (-1 == ChangeDir(cmd[1])) {
                        exit(EXIT_FAILURE);
                    }
                }

                else if (-1 == execvp(cmd[0], cmd)) {
                    char errcmd[cap];
                    strcpy(errcmd, cmd[0]);
                    perror(errcmd);
                    exit(EXIT_FAILURE);
                }
                exit(1); // prevents zombie process
            }

            else {
                // save stdin only in the first command
                if (savestdin == -1) {
                    if (-1 == (savestdin = dup(0))) {
                        perror("dup()");
                        exit(1);
                    }
                }
                // close PIPE_WRITE because I'm not gonna using it
                if (-1 == close(fd[PIPE_WRITE])) {
                    perror("close()");
                    exit(1);
                }
                // change stdin to PIPE_READ so the next command can read from the pipe
                if (-1 == dup2(fd[PIPE_READ], 0)) {
                    perror("dup2()");
                    exit(1);
                }
                for (int i = 0; i < count; ++i) {
                    if (cmd[i] != 0) {
                        delete[] cmd[i];
                        cmd[i] = 0;
                    }
                }
                // recursion from here
                Piping(cmdLine, isExecuted, flag, savestdin);
                // restore stdin or can't exit the child when a file is
                // too large to store in the pipe
                if (-1 == dup2(savestdin, 0)) {
                    perror("dup2()");
                    exit(1);
                }
                // close PIPE_READ after each fork or the child can't exit when a large file
                // comes in the pipe
                if (-1 == close (fd[PIPE_READ])) {
                    perror("close()");
                    exit(1);
                }
                errno = 0;
                // wait after each fork so that every child process can run simultaneously
                if (-1 == waitpid(-1, &status, WUNTRACED) && errno == EINTR) {
                    if (-1 == waitpid(-1, &status, WUNTRACED)) {
                        perror("wait()");
                        exit(1);
                    }
                }
                if (errno != 0 && errno != EINTR) {
                    perror("wait()");
                    exit(1);
                }
            }
        }
    }

    return;
}

int ChangeDir(const char *path) {
    string newPath;
    char tempPath[256];
    char *homePath;
    char *oldPath;
    char *pathName[128] = {0};
    vector<string> simplePath;

    if (path == NULL) {
        if (NULL == (homePath = getenv("HOME"))) {
            perror("getenv()");
            exit(1);
        }
        strcpy(tempPath, homePath);
        newPath = homePath;
    }

    else if (strcmp(path, "-") == 0) {
        if (NULL == (oldPath = getenv("OLDPWD"))) {
            perror("getenv()");
            exit(1);
        }
        strcpy(tempPath, oldPath);
        newPath = oldPath;
        cout << newPath << endl;
    }

    else {
        if (path[0] == '/') {
            strcpy(tempPath, path);
        }
        else {
            strcpy(tempPath, currPath);
            strcat(tempPath, "/");
            strcat(tempPath, path);
        }
        char pathCpy[256];
        unsigned int i = 0;

        strcpy(pathCpy, tempPath);
        pathName[i] = strtok(pathCpy, "/");
        while (pathName[i] != NULL) {
            ++i;
            pathName[i] = strtok(NULL, "/");
        }

        for (i = 0; pathName[i] != NULL; ++i) {
            if (strcmp(pathName[i], "..") == 0 && !simplePath.empty()) {
                simplePath.pop_back();
            }
            else if (strcmp(pathName[i], ".") != 0 && strcmp(pathName[i], "..") != 0) {
                simplePath.push_back(pathName[i]);
            }
        }

        if (simplePath.empty()) {
            newPath = "/";
        }
        else {
            for (i = 0; i < simplePath.size(); ++i) {
                newPath = newPath + '/' + simplePath.at(i);
            }
        }
    }

    if (-1 == chdir(tempPath)) {
        char errmsg[128] = "cd: ";
        strcat(errmsg, path);
        perror(errmsg);
        return -1;
    }

    if (-1 == setenv("PWD", newPath.c_str(), 1)) {
        perror("setenv()");
        exit(1);
    }
    if (-1 == setenv("OLDPWD", currPath, 1)) {
        perror("setenv()");
        exit(1);
    }

    return 0;
}

void SimplifyPath(string &simplePath) {
    char *homePath;
    if (NULL == (homePath = getenv("HOME"))) {
        perror("getenv()");
        exit(1);
    }
    string homestr = homePath;
    long unsigned pos = 0;
    if ((pos = simplePath.find(homestr)) != string::npos) {
        simplePath.replace(pos, homestr.size(), "~");
    }
}

void interrupthdl(int signum, siginfo_t *info, void *ptr) {
    if (childpid != -1) {
        if (-1 == kill(childpid, SIGKILL)) {
            perror("kill()");
//            exit(1);
        }
        isKilled = true;
    }
    cout << endl;
    return;
}

void stophdl(int signum, siginfo_t *info, void *ptr) {
    if (childpid != -1) {
        stopchild.push_back(childpid);
        if (-1 == kill(childpid, SIGSTOP)) {
            perror("kill()");
//            exit(1);
        }
    }
    cout << endl;
    return;
}

int Foreground(int bg) {
    if (!stopchild.empty()) {
        if (-1 == kill(stopchild.back(), SIGCONT)) {
            perror("kill()");
            exit(1);
        }
        childpid = stopchild.back();
        if (bg == 1) {
            return 0;
        }
        errno = 0;
        if (-1 == waitpid(childpid, &status, WUNTRACED) && errno == EINTR) {
            if (-1 == waitpid(childpid, &status, WUNTRACED)) {
                perror("wait()");
                exit(1);
            }
        }
        if (errno != 0 && errno != EINTR) {
            perror("wait()");
            exit(1);
        }

        stopchild.pop_back();
        return 0;
    }
    else {
        return -1;
    }
}



