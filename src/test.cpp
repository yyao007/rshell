#include <iostream>
#include <string>
#include <cstring>

using namespace std;

int main() {
    char str[100];

    cin.getline(str, 100);

    if (str[0] == '\0') {
        cout << "no input: " << str << endl;
        return 0;
    }
    cout << "input is: " << str << endl;

    return 0;
}
