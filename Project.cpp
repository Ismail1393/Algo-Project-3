#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

int main() {
    const string filename = "tiny.txt"; // Replace with your file name

    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cout << "Unable to open file " << filename << endl;
        return 1;
    }

   
    string line;
    while (getline(inputFile, line)) {
        istringstream iss(line);
        cout << "ISS" << iss << endl;

    // Displaying the values stored in the 2D array
    cout << "Values stored in the 2D array:" << endl;
    for (const auto &row : values) {
        for (int val : row) {
            cout << val << " ";
        }
        cout << endl;
    }

    inputFile.close();

    return 0;
}
