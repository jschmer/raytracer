#define _USE_MATH_DEFINES
#include <string>
#include <iostream>
#include <Windows.h>

#include <RayTracer\Scene.h>

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
    bool folder = true;

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
    else if (folder) {
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
        scene.loadScene("testscenes/scene3.test");

        std::cout << "output name: " << scene._outputFilename << "\n";

        scene.render();
    }

    std::cout << "Press a key...\n";
    getchar();
    return 0;
}