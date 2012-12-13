#include <glm\glm.hpp>
#include <iostream>
#include <iomanip>
#include <string>

using namespace glm;
using namespace std;

float vecLen(vec3 &vec) {
    return sqrt(pow(vec[0], 2.0f)+pow(vec[1], 2.0f)+pow(vec[2], 2.0f));
}

void printMat4(mat4 &mat, std::string title) {
    cout << title << endl;
    for (int i=0; i<4; ++i) {
        for (int j=0; j<4; ++j) {
            if (mat[j][i] >= 0)
                cout << ' ';
            cout << setw(4) << setfill(' ') << fixed << setprecision(3) << mat[j][i] << " ";
        }
        cout << endl;
    }
}

void printVec4(vec4 &vec, std::string title) {
    cout << title << endl;
    for (int j=0; j<4; ++j) {
        if (vec[j] >= 0)
                cout << ' ';
        cout << setw(4) << setfill(' ') << fixed << setprecision(3) << vec[j] << endl;
    }
    cout << endl;
}