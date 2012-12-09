#define _USE_MATH_DEFINES
#include <string>
#include <iostream>
#include <Windows.h>

#define GLM_SWIZZLE_XYZW 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
using namespace glm;

#include <RayTracer\Scene.h>
#include <RayTracer\Helper.h>

#include <sstream>

void getAllFilesIn(std::string folder, std::vector<std::string> &files) {
    HANDLE hFind;
    WIN32_FIND_DATA data;

    hFind = FindFirstFile((folder + "*.test").c_str(), &data);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (data.cFileName[0] != '.')
                files.push_back(std::string(data.cFileName));
        } while (FindNextFile(hFind, &data));
        FindClose(hFind);
    }
}

int main(int argc, char* argv[]) {
    bool batch = false;
    bool submission = false;

    // vektoren von rechts multiplizieren!
    // matrix stack: rechte transformation kommt zuerst!!

    if (false) {
        vec4 pos(1, 0, 1, 1);   // scale (2, 1, 3)
        // -> vec4 pos(2, 0, 3, 1);   // translate (-1, 1, -3)
        // -> vec4 pos(1, 1, 0, 1);   // rotate 90° um (0, 1, 0)
        // -> vec4 pos(0, 1, -1, 1);

        mat4 transf(1);
        printVec4(pos, "pos = no transform");

        // scale
        transf = glm::scale(mat4(1), vec3(2, 1, 3)) * transf;
        printVec4(transf * pos, "pos = scale");

        // translate
        transf = glm::translate(mat4(1), vec3(-1, 1, -3)) * transf;
        printVec4(transf * pos, "pos = scale+translate");

        // rotate
        transf = glm::rotate(mat4(1), 90.0f, vec3(0, 1, 0)) * transf;
        printVec4(transf * pos, "pos = scale+translate+rotate");

    }
    if (batch) {
        for (int i=1; i<5; ++i) {
            stringstream s;
            s << i;

            Scene scene;
            scene.loadScene("testscenes/scene1-" + s.str() + ".test");

            std::cout << "output name: " << scene._outputFilename << "\n";

            scene.render();
        }
    } 
    else if (submission) {
        std::vector<std::string> files;
        std::string folder = "hw3-submissionscenes\\";
        getAllFilesIn(folder, files);

        for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
            Scene scene;
            scene.loadScene(folder + *it);
            std::cout << "output name: " << scene._outputFilename << "\n";
            scene.render();  
        }

    }
    else {
        Scene scene;
        scene.loadScene("testscenes/scene4-specular.test");

        std::cout << "output name: " << scene._outputFilename << "\n";

        scene.render();
    }

    std::cout << "Press a key...\n";
    getchar();
    return 0;
}