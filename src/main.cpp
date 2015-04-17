#include <iostream>
#include <string>
#include <string.h>

using namespace std;

int main() {
    string str = "hello world";
    char a[100];
    char *b;

    strcpy(a, str.c_str());

    b = strtok(a, " ");
    str = b;
    cout << str;
    return 0;
}

