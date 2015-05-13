#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <errno.h>
using namespace std;

void RemoveFiles(vector<char*>&);
void RemoveDir(vector<char*>&);

int main(unsigned int argc, char** argv) {
    unsigned int i;
    unsigned int j;
    vector<char*> flags;
    vector<char*> files;

    for (i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            flags.push_back(argv[i]);
        }
        else {
            files.push_back(argv[i]);
        }
    }

    if (flags.size() == 0) {
    }

    else {
        for (i = 0; i < flags.size(); ++i) {
            for (j = 1; flags.at(i)[j] != '\0' && flags.at(i)[j] != 'r'; ++j) {
                cout << "rm: invalid flags"  << endl;
                exit(1);
            }
        }



    }

    return 0;
}

void RemoveFiles(vector<char*> &files) {
    for (unsigned int i = 0; i < files.size(); ++i) {
        if (-1 == unlink(files.at(i))) {
            char temp[] = "rm: cannot remove ";
            strcat(temp, files.at(i));
            perror(temp);
        }
        else {
            files.earse(files.begin() + i);
        }
    }
}

void RemoveDir(vector<char*> dir) {
    DIR* dirp;
    for (unsigned int i = 0; i < dir.size(); ++i) {
        if (NULL == (dirp = opendir(dir.at(i)))) {
            perror ("rmdir()");
        }
        struct dirent* filespecs;
        errno = 0;


