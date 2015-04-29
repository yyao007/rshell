#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
using namespace std;

struct stat st;

int main() {

    stat("a.out", &st);
    struct passwd *pw = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);
    cout << pw->pw_name << endl;
    cout << gr->gr_name << endl;
    cout << st.st_size << endl;

    return 0;
}
