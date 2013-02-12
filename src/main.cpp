#define _USE_MATH_DEFINES
#include <string>
#include <iostream>
#include <Windows.h>
#include <sstream>
#include <Shlwapi.h>

#define GLM_SWIZZLE_XYZW 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
using namespace glm;

//#include <RayTracer/Scene/Scene.h>
#include <RayTracer/RenderTarget/RayTraceImage.h>
#include <RayTracer/RayTracer.h>
#include <String/StringHelper.h>

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

void printUsage() {
    std::string usage = "\
Usage:\n\
    raytracer.exe filename\n\
    raytracer.exe directory file_extenstion\n\
\n\
Examples:\n\
    raytracer.exe Shuriken.obj\n\
    raytracer.exe scenes/edx test\n\
";
    std::cout << usage;
}

int main(int argc, char* argv[]) {
    std::string path = "";
    std::string ext  = "";

    switch (argc) {
    case 2:
        path = std::string(argv[1]);
        break;
    case 3:
        path = std::string(argv[1]);
        ext  = std::string(argv[2]);
        break;
    default:
        printUsage();
        std::cout << "Press any key to continue..." << std::endl;
        getchar();
        return -1;
    }

    // more portable and failsafe: forward slash!
    path = String::replace(path, "\\", "/");
   
    // append a forward slash if we are in directory mode
    if (argc != 2 && path != "" && path.at(path.length()-1) != '/')
        path += '/';

    // do the rendering for all files in path with ext extension
    if (ext.length() > 0) {
        // get all filenames
        std::vector<std::string> files;
        
        getAllFilesIn(path, files, ext);

        if (files.size() == 0)
            std::cout << "No files found!" << std::endl;

        // render all the files
        for (auto it = files.begin(); it != files.end(); ++it) {
            auto& filepath = path + *it;
            std::cout << "Rendering " << filepath << std::endl;

            auto out_file = String::replaceExtension(filepath, "png");
            std::cout << "    into: " << out_file << "\n";

            RayTraceImage image(out_file, 400, 400);
            RayTracer tracer;
            tracer.load(filepath);
            tracer.renderInto(&image);

            std::cout << "    Done!" << std::endl << std::endl;
        }
    }
    else {
        std::cout << "Rendering " << path << std::endl;

        auto out_file = String::replaceExtension(path, "png");
        std::cout << "    into: " << out_file << "\n";

        RayTraceImage image(out_file, 400, 400);
        RayTracer tracer;
        tracer.load(path);
        tracer.renderInto(&image);

        std::cout << "    Done!" << std::endl << std::endl;
    }
    
    return 0;
}






















// Matrix Transformation Turotial :D
void MatrixOps() {
    // vektoren von rechts multiplizieren!
    // matrix stack: rechte transformation kommt zuerst!!

    vec4 pos(1, 0, 1, 1);   // scale (2, 1, 3)
    // -> vec4 pos(2, 0, 3, 1);   // translate (-1, 1, -3)
    // -> vec4 pos(1, 1, 0, 1);   // rotate 90° um (0, 1, 0)
    // -> vec4 pos(0, 1, -1, 1);

    mat4 transf(1);
    //printVec4(pos, "pos = no transform");

    // scale
    transf = glm::scale(mat4(1), vec3(2, 1, 3)) * transf;
    //printVec4(transf * pos, "pos = scale");

    // translate
    transf = glm::translate(mat4(1), vec3(-1, 1, -3)) * transf;
    //printVec4(transf * pos, "pos = scale+translate");

    // rotate
    transf = glm::rotate(mat4(1), 90.0f, vec3(0, 1, 0)) * transf;
    //printVec4(transf * pos, "pos = scale+translate+rotate");

    //printVec4(vec4(), "Vec4");
}
