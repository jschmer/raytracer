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
#include <Shlwapi.h>

void getAllFilesIn(std::string folder, std::vector<std::string> &files, std::string ext = "test") {
    HANDLE hFind;
    WIN32_FIND_DATA data;

    hFind = FindFirstFile((folder + "*."+ext).c_str(), &data);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (data.cFileName[0] != '.')
                files.push_back(std::string(data.cFileName));
        } while (FindNextFile(hFind, &data));
        FindClose(hFind);
    }
}

void MatrixOps() {
    // vektoren von rechts multiplizieren!
    // matrix stack: rechte transformation kommt zuerst!!

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

    printVec4(vec4(), "Vec4");
}

int main(int argc, char* argv[]) {
    bool dir = false;
    bool useExt = false;

    std::string path = "";
    std::string ext = "";
    if (argc == 2) {
        path = std::string(argv[1]);
    }
    else if (argc == 3) {
        path = std::string(argv[1]);
        ext = std::string(argv[2]);
        useExt = true;
    }
    
    // append a backslash
    if (argc != 2 && path != "" && path.at(path.length()-1) != '\\' && path.at(path.length()-1) != '/')
        path += '\\';

    DWORD attr = 0;
    if (attr = GetFileAttributes(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
        if (attr & FILE_ATTRIBUTE_DIRECTORY) {
            dir = true;
        } else if (attr & FILE_ATTRIBUTE_NORMAL) {
            ;
        }
    }
    else {
        std::cout << "Error getting file attributes for: " << path << std::endl;
        getchar();
        return -1;
    }

    std::vector<std::string> files;
    if (dir) {
        // read all *.test files in directory
        if (useExt)
            getAllFilesIn(path, files, ext);
        else
            getAllFilesIn(path, files);

        if (files.size() == 0)
            std::cout << "No files found!" << std::endl;

        // render all the files
        for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
            std::cout << "Rendering " << path + *it << std::endl;

            Scene scene;
            scene.load(path + *it);
            std::cout << "\tOutput name: " << scene._outputFilename << "\n";
            scene.render();  
        }
    }
    else {
        std::cout << "Rendering " << path << std::endl;

        // render only the specified file
        Scene scene;
        scene.load(path);
        std::cout << "\tOutput name: " << scene._outputFilename << "\n";
        scene.render();  
    }
    
    std::cout << "Press a key...\n";
    //getchar();
    return 0;
}