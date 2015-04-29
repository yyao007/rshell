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

struct convertStat {
    struct stat buf;
    string userName;
    string groupName;
    char timeStr[256];
};


void PrintL(const vector<string> &file, const string &dirName) {
    struct passwd *pw;
    struct group *gr;
    struct tm *Tm;
    unsigned int max[3] = {0};
    unsigned int i = 0;
    unsigned int length = 0;
    unsigned int temp = 0;
    vector<convertStat> st(file.size());
    string pathName;

    for (i = 0; i < file.size(); ++i) {
        pathName = dirName + '/' + file.at(i);
        if (-1 == stat(pathName.c_str(), &st.at(i).buf)) {
            perror("stat()");
            exit(1);
        }

        errno = 0;
        if (NULL != (pw = getpwuid(st.at(i).buf.st_uid))) {
            st.at(i).userName = pw->pw_name;
            if (max[0] < st.at(i).userName.size()) {
                max[0] = st.at(i).userName.size();
            }
        }
        if (errno != 0) {
            perror("getpwuid()");
            exit(1);
        }

        errno = 0;
        if (NULL != (gr = getgrgid(st.at(i).buf.st_gid))) {
            st.at(i).groupName = gr->gr_name;
            if (max[1] < st.at(i).groupName.size()) {
                max[1] = st.at(i).groupName.size();
            }
        }
        if (errno != 0) {
            perror("getgrgid()");
            exit(1);
        }

        errno = 0;
        time_t t = st.at(i).buf.st_mtime;
        if (NULL != (Tm = localtime(&t))) {
            if (0 == strftime(st.at(i).timeStr,
                sizeof(st.at(i).timeStr), "%b %d %H:%M ", Tm))
            {
                perror("strftime()");
                exit(1);
            }
        }
        if (errno != 0) {
            perror("localtime()");
            exit(1);
        }

        if (length < st.at(i).buf.st_size) {
            length = st.at(i).buf.st_size;
            temp = length;
            for (max[2] = 0; temp > 0; ++max[2]) {
                temp /= 10;
            }
        }
    }

    for (i = 0; i < file.size(); ++i) {
//        pathName = dirName + '/' + file.at(i);
//        if (-1 == stat(pathName.c_str(), &buf)) {
//            perror("stat()");
//            exit(1);
//        }

        cout << ((S_ISDIR(st.at(i).buf.st_mode))? "d":"-") <<
        ((st.at(i).buf.st_mode & S_IRUSR)? "r":"-") <<
        ((st.at(i).buf.st_mode & S_IWUSR)? "w":"-") <<
        ((st.at(i).buf.st_mode & S_IXUSR)? "x":"-") <<
        ((st.at(i).buf.st_mode & S_IRGRP)? "r":"-") <<
        ((st.at(i).buf.st_mode & S_IWGRP)? "w":"-") <<
        ((st.at(i).buf.st_mode & S_IXGRP)? "x":"-") <<
        ((st.at(i).buf.st_mode & S_IROTH)? "r":"-") <<
        ((st.at(i).buf.st_mode & S_IWOTH)? "w":"-") <<
        ((st.at(i).buf.st_mode & S_IXOTH)? "x":"-") << ' ';
        cout << setw(max[0]) << st.at(i).userName << ' ';
        cout << setw(max[1]) << st.at(i).groupName << ' ';
        cout << setw(max[2]) << st.at(i).buf.st_size << ' ';
        cout << st.at(i).timeStr << file.at(i) << endl;
    }
    return;
}

int main() {
    const char* f[] = {"bin", "case", "LICENSE", "Makefile", "README.md", "src", "tests"};
    vector<string> files(f, f + 7);
    PrintL(files, "..");

    return 0 ;
}



