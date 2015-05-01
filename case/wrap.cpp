#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

unsigned int maxSize(const vector<string> &fileNames) {
    unsigned int max = 0;
    unsigned int i;
    for (i = 0; i < fileNames.size(); ++i) {
        if (max < fileNames.at(i).size()) {
            max = fileNames.at(i).size();
        }
    }
    return max;
}

void PrintSingleDir(const vector<string> &file) {
    unsigned int i, j;
    const unsigned int width = 80;
    unsigned int oneLine;
    unsigned int begin = 0;
    vector< vector<string> > mat;
    vector<string> buf;
    unsigned int rows = 0;
    unsigned int cols = 0;
    unsigned int max[80];
    unsigned int colume;

    while (oneLine > width + 2) {
        mat.clear();
        ++rows;
        oneLine = 0;
        begin = 0;
        while (begin < file.size()) {
            for (i = begin; i < begin + rows && i < file.size(); ++i) {
                buf.push_back(file.at(i));
            }
            begin = i;
            mat.push_back(buf);
            buf.clear();
        }
        cols = mat.size();

        for (i = 0; i < cols; ++i) {
            max[i] = maxSize(mat.at(i));
        }

        for (i = 0; i < cols; ++i) {
            oneLine += max[i] + 2;
        }
    }

    for (i = 0; i < rows; ++i) {
        if (mat.at(cols - 1).size() <= i) {
            colume = cols - 1;
        }
        else {
            colume = cols;
        }

        for (j = 0; j < colume; ++j) {
            cout << setw(max[j]) << mat.at(j).at(i) << "  ";
        }
        cout << endl;
    }
    return;
}


int main() {
    const char* f[] = {"aaaaa","bbbbbb","ccccccc","ddddd","eeeee","fffff","gggg",
    "hhhhh","iiiii","jjjjj","asdsafbaghegiqohpiqojdpiqjwpidqjpidjpqjdpn","kkkkkkkkkkkkkkkkkkkk","asdasd","asdsadaaaaaa",  "lllll","mmmmm","nnnnn"};
    vector<string> files(f, f + 17);

    PrintSingleDir(files);

    return 0;
}
