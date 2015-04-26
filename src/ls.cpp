#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <pwd.h>
#include <grp.h>
#include <time.h>
using namespace std;

#define FLAG_a 0b001
#define FLAG_l 0b010
#define FLAG_R 0b100

struct dirent *filespecs;

int getFlag(const vector<string> &);
void SortFile(vector<string> &, int);
int ReadDir(vector<string> &, const char*, int);
void PrintL(const vector<string> &, const string &);
void PrintSingleDir(const vector<string> &);


int main(int argc, char** argv) {
    int i, j;
    int flag;
    int errno;
    vector<string> dirn; // directory name list
    vector<string> dirf; // user input flag "-a -l -R"
    vector<string> files; // store each file name

    for (i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            dirn.push_back(argv[i]);
        }
        else {
            dirf.push_back(argv[i]);
        }
    }

    flag = getFlag(dirf);

    if (dirn.size() == 0) {
        dirn.push_back(".");
    }

    sort(dirn.begin(), dirn.end());

    for (i = 0; i < dirn.size(); ++i) {
        if (-1 == (errno = ReadDir(files, dirn.at(i).c_str(), flag))) {
            dirn.erase(dirn.begin() + i);
            --i;
        }
        files.clear();
    }
    for (i = 0; i < dirn.size(); ++i) {
        errno = ReadDir(files, dirn.at(i).c_str(), flag);
        if (flag & FLAG_l) {
            if (flag & FLAG_R) {

            }
            else {
                cout << dirn.at(i) << ":" << endl;
                PrintL(files, dirn.at(i));
                if (i < dirn.size() - 1) {
                    cout << endl;
                }
            }
        }
        else {
            if (flag & FLAG_R) {

            }
            else {
                if (dirn.size() > 1) {
                    cout << dirn.at(i) << ":" << endl;
                }
                PrintSingleDir(files);
                if (i < dirn.size() - 1) {
                    cout << endl;
                }
            }
        }
        files.clear();
    }

    return 0;
}

int getFlag(const vector<string> &f) {
    int i, j;
    int flag = 0b000;
    for (i = 0; i < f.size(); ++i) {
        for (j = 0; j < f.at(i).size(); ++j) {
            if (f.at(i).at(j) == 'a') {
                flag = flag | FLAG_a;
            }
            else if (f.at(i).at(j) == 'l') {
                flag = flag | FLAG_l;
            }
            else if (f.at(i).at(j) == 'R') {
                flag = flag | FLAG_R;
            }
        }
    }
    return flag;
}

void SortFile(vector<string> &file, int flag) {
    int i;
    if (!(flag & FLAG_a)) {
        for (i = 0; i < file.size(); ++i) {
            if (file.at(i).at(0) == '.') {
                file.erase(file.begin() + i);
                --i;
            }
        }
    }
    sort(file.begin(), file.end());
    return;
}

int ReadDir(vector<string> &file, const char* dirct, int flag) {
    DIR* dirp;
    char temp[] = "ls: cannot access ";
    if (NULL == (dirp = opendir(dirct))) {
        strcat(temp, dirct);
        perror(temp);
        return -1;
    }
    errno = 0;
    while (NULL != (filespecs = readdir(dirp))) {
        file.push_back(filespecs -> d_name);
    }
    if (errno != 0) {
        perror("readdir()");
        exit(1);
    }
    SortFile(file, flag);

    if (-1 == closedir(dirp)) {
        perror("closedir()");
        exit(1);
    }
    return 1;
}

void PrintSingleDir(const vector<string> &file) {
    int i;
    for (i = 0; i < file.size(); ++i) {
        cout << file.at(i) << "  ";
    }
    cout << endl;
    return;
}

void PrintL(const vector<string> &file, const string &dirName) {
    int i;
    int length = 0;
    int temp = 0;
    int max[3] = {0};
    char timestr[128];
    string pathName;
    vector<string> user;
    vector<string> group;
    struct stat buf;
    struct passwd *pw;
    struct group *gr;
    struct tm *tm;

    for (i = 0; i < file.size(); ++i) {
        pathName = dirName + '/' + file.at(i);
        if (-1 == stat(pathName.c_str(), &buf)) {
            perror("stat");
            return;
        }
        pw = getpwuid(buf.st_uid);
        gr = getgrgid(buf.st_gid);
        user.push_back(pw->pw_name);
        group.push_back(gr->gr_name);

        if (length < buf.st_size) {
            length = buf.st_size;
            temp = length;
            for (max[0] = 0; temp > 0; ++max[0]) {
                temp /= 10;
            }
        }
        if (max[1] < user.at(i).size()) {
            max[1] = user.at(i).size();
        }
        if (max[2] < group.at(i).size()) {
            max[2] = group.at(i).size();
        }
    }
    for (i = 0; i < file.size(); ++i) {
        pathName = dirName + '/' + file.at(i);
        stat(pathName.c_str(), &buf);
        tm = localtime(&buf.st_mtime);
        strftime(timestr, sizeof(timestr), "%b %d %H:%M ", tm);

        cout << ((S_ISDIR(buf.st_mode))? "d":"-") <<
        ((buf.st_mode & S_IRUSR)? "r":"-") <<
        ((buf.st_mode & S_IWUSR)? "w":"-") <<
        ((buf.st_mode & S_IXUSR)? "x":"-") <<
        ((buf.st_mode & S_IRGRP)? "r":"-") <<
        ((buf.st_mode & S_IWGRP)? "w":"-") <<
        ((buf.st_mode & S_IXGRP)? "x":"-") <<
        ((buf.st_mode & S_IROTH)? "r":"-") <<
        ((buf.st_mode & S_IWOTH)? "w":"-") <<
        ((buf.st_mode & S_IXOTH)? "x":"-") << ' ';
        cout << setw(max[1]) << user.at(i) << ' ';
        cout << setw(max[2]) << group.at(i) << ' ';
        cout << setw(max[0]) << buf.st_size << ' ';
        cout << timestr << file.at(i) << endl;
    }
    return;
}




