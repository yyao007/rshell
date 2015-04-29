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

#define FLAG_a 0x001
#define FLAG_l 0x010
#define FLAG_R 0x100

struct convertStat {
   // struct stat;
    string userName;
    string groupName;
    char timeStr[256];
};

int getFlag(const vector<string> &);
void SortFile(vector<string> &, const int);
int ReadDir(vector<string> &, const char*, const int);
void PrintL(const vector<string> &, const string &);
void PrintSingleDir(const vector<string> &);
void PrintR(vector<string> &, const string &, const int);

int main(int argc, char** argv) {
    unsigned int i;
    int flag;
    vector<string> dirn; // directory name list
    vector<string> dirf; // user input flag "-a -l -R"
    vector<string> files; // store each file name

    for (i = 1; i < static_cast<unsigned int>(argc); ++i) {
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
        if (-1 == ReadDir(files, dirn.at(i).c_str(), flag)) {
            dirn.erase(dirn.begin() + i);
            --i;
        }
        files.clear();
    }
    for (i = 0; i < dirn.size(); ++i) {
        if (-1 == ReadDir(files, dirn.at(i).c_str(), flag)) {
            continue;
        }
        if (flag & FLAG_R) {
            PrintR(files, dirn.at(i), flag);
        }
        else {
            if (dirn.size() > 1) {
                cout << dirn.at(i) << ":" << endl;
            }
            if (flag & FLAG_l) {
                PrintL(files, dirn.at(i));
            }
            else {
                PrintSingleDir(files);
            }
        }
        if (i < dirn.size() - 1) {
            cout << endl;
        }
        files.clear();
    }

    return 0;
}

int getFlag(const vector<string> &f) {
    unsigned int i, j;
    int flag = 0x000;
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

void SortFile(vector<string> &file, const int flag) {
    unsigned int i;
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

int ReadDir(vector<string> &file, const char* dirct, const int flag) {
    DIR* dirp;
    char temp[] = "ls: cannot access ";
    if (NULL == (dirp = opendir(dirct))) {
        strcat(temp, dirct);
        perror(temp);
        return -1;
    }
    struct dirent *filespecs;
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
    unsigned int i;
/*    int width = 80;
    string buf;

    i = 0;
    while (buf.size() <= 80 && i < file.size()) {
        if (buf.size() + file.at(i).size() <= 80) {
            buf += file.at(i) + "  ";
        }

        ++i;
    }
*/

    if (file.size() != 0) {
        for (i = 0; i < file.size(); ++i) {
            cout << file.at(i) << "  ";
        }
        cout << endl;
    }
    return;
}

void PrintL(const vector<string> &file, const string &dirName) {
    struct stat buf;
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
        if (-1 == stat(pathName.c_str(), &buf)) {
            perror("stat()");
            exit(1);
        }

        errno = 0;
        if (NULL != (pw = getpwuid(buf.st_uid))) {
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
        if (NULL != (gr = getgrgid(buf.st_gid))) {
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
        if (NULL != (Tm = localtime(&buf.st_mtime))) {
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

        if (length < buf.st_size) {
            length = buf.st_size;
            temp = length;
            for (max[2] = 0; temp > 0; ++max[2]) {
                temp /= 10;
            }
        }
    }

    for (i = 0; i < file.size(); ++i) {
        pathName = dirName + '/' + file.at(i);
        if (-1 == stat(pathName.c_str(), &buf)) {
            perror("stat()");
            exit(1);
        }

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
        cout << setw(max[0]) << st.at(i).userName << ' ';
        cout << setw(max[1]) << st.at(i).groupName << ' ';
        cout << setw(max[2]) << buf.st_size << ' ';
        cout << st.at(i).timeStr << file.at(i) << endl;
    }
    return;
}

void PrintR(vector<string> &file, const string &dirName, const int flag) {
    struct stat st;
    unsigned int i;
    string pathName;
    vector<string> dirn;

    for (i = 0; i < file.size(); ++i) {
        if (file.at(i) != ".." && file.at(i) != ".") {
            pathName = dirName + '/' + file.at(i);

            if (-1 == stat(pathName.c_str(), &st)) {
                perror("stat()");
                exit(1);
            }
            if (S_ISDIR(st.st_mode)) {
                dirn.push_back(pathName);
            }
        }
    }

    // choose print method according to the user input flag
    cout << dirName << ":" << endl;
    if (flag & FLAG_l) {
        PrintL(file, dirName);
    }
    else {
        PrintSingleDir(file);
    }
    // clear file before each recursion
    file.clear();
    // Recursion through each folder
    for (i = 0; i < dirn.size(); ++i) {
        if (-1 == ReadDir(file, dirn.at(i).c_str(), flag)) {
            return;
        }
        cout << endl;
        PrintR(file, dirn.at(i), flag);
    }
    return;
}

