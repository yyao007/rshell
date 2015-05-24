#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <vector>

using namespace std;

int ChangeDir(const char *, const char *);

int main(int argc, char **argv) {
    while (1) {

    char *currPath = getenv("PWD");
    if (currPath == NULL) {
        perror("getenv()");
        exit(1);
    }
    cout << currPath << ": ";
    char path[128];
    cin.getline(path, 128);
    ChangeDir(path, currPath);

    }
    return 0;
}

int ChangeDir(const char *path, const char *currPath) {
    string newPath;
    char tempPath[256];
    char *pathName[128] = {0};
    vector<string> simplePath;

    strcpy(tempPath, currPath);
    strcat(tempPath, "/");
    strcat(tempPath, path);

    // split the whole path name into many directories
    int i = 0;
    pathName[i] = strtok(tempPath, "/");
    while (pathName[i] != NULL) {
        ++i;
        pathName[i] = strtok(NULL, "/");
    }

    for (i = 0; pathName[i] != NULL; ++i) {
        if (strcmp(pathName[i], "..") == 0) {
            simplePath.pop_back();
        }
        else if (strcmp(pathName[i], ".") != 0) {
            simplePath.push_back(pathName[i]);
        }
    }
    // get the simple path name
    for (i = 0; i < simplePath.size(); ++i) {
        newPath = newPath + '/' + simplePath.at(i);
    }

    // use the original path to check if there's a invalid directory
    strcpy(tempPath, currPath);
    strcat(tempPath, "/");
    strcat(tempPath, path);
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

