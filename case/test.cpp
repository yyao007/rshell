#include <iostream>
#include <string>
#include <cstring>

using namespace std;

int main() {
    char str[100];
    char *estr;

    cin.getline(str, 100);

    cout << "your string is: " << str << endl;
    estr = strtok(str, " ");

    cout << "after strtok is: " << str << endl;
    cout << "strtok is: " << estr << endl;

/*    if (str[0] == '\0') {
        cout << "no input: " << str << endl;
        return 0;
    }
    cout << "input is: " << str << endl;
*/
    return 0;
}
