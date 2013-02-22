#include <RayTracer/SceneReader/RaytraceParser.h>

#include <string>
#include <sstream>
//#include <iostream>
#include <fstream>
//#include <stack>

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale

#include <RayTracer/glm_includes.h>
#include <RayTracer/Scene/Scene.h>
#include <RayTracer/Scene/Camera.h>
#include <RayTracer/Scene/Material.h>

//#include <RayTracer/Scene/Primitives/Sphere.h>

#include <RayTracer/SceneReader/SceneReader.h>
#include <RayTracer/SceneReader/SceneParserHelper.h>

using namespace std;

std::unique_ptr<Scene> RaytraceParser::load(std::unique_ptr<Scene> scene) const {
    std::string sceneFile_dir = getDirectory(sceneFile);

    string str, cmd; 
    ifstream in;
    in.open(sceneFile); 
    if (in.is_open()) {
        // matrix stack to store transforms.  
        //stack <mat4> transfstack; 
        //transfstack.push(mat4(1.0f));  // identity

        vector<vec3> vertices;

        Material* mat = new Material;
        mat->diffuse = vec3(0);
        bool push_mat = true;

        bool last = false;   // transformation applied last

        bool scene_embedded = false;

        // constant, linear, quadratic attenuations 
        vec3 attenuation(1.0, 0.0, 0.0);

        std::getline(in, str); 
        while (in) {
            if ((str.find_first_not_of(" \t/r\n") != string::npos) && (str[0] != '#')) {
                // Ruled out comment and blank lines 

                stringstream s(str);
                s >> cmd; 
                vector<float> values;
                bool validinput; // Validity of input 

                // embed another scene file
                if (!scene_embedded && cmd == "use_scene") {
                    auto scene_file_to_embed = s.str();
                    scene_file_to_embed.erase(0, cmd.length() + 1);

                    scene = loadScene(sceneFile_dir + scene_file_to_embed);
                }

                // maximum number of reflection rays
                if (cmd == "maxdepth") {
                    validinput = readvals(s, 1, values); 
                    if (validinput) { 
                        scene->_maxdepth = static_cast<int>(values[0]);
                    } 
                }

                // output filename
                else if (cmd == "output") {
                    s >> scene->_outputFilename; 
                }
                // output image size
                else if (cmd == "size") {
                    validinput = readvals(s, 2, values); 
                    if (validinput) {
                        scene->_size.width  = static_cast<int>(values[0]);
                        scene->_size.height = static_cast<int>(values[1]);
                        scene->_hasSize = true;
                    }
                }

                // camera spec
                else if (cmd == "camera") {
                    validinput = readvals(s, 10, values); // 10 values eye cen up fov
                    if (validinput) {
                        // camera       0 0 5 0 0 0 0 1 0 90.0
                        // look from    0 0 5   eye
                        // look at      0 0 0   origin
                        // up           0 1 0   up
                        // fovy         90.0

                        // camera 0 -2 2 0 0 0 0 1 1 30.0
                        vec3 eye    = vec3(values[0], values[1], values[2]);
                        vec3 center = vec3(values[3], values[4], values[5]);
                        vec3 up     = vec3(values[6], values[7], values[8]);
                        float fovy  = values[9];

                        if (scene->_camera)
                            delete scene->_camera;
                        Camera *c = new Camera(eye, center, up, fovy);
                        scene->_camera = c;
                    }
                }

                // Process the lights, add it to database.
                else if (cmd == "directional") {
                    validinput = readvals(s, 6, values); // Position/color for lts.
                    if (validinput) {
                        vec4 dir(values[0], values[1], values[2], 0);
                        vec3 color(values[3], values[4], values[5]);

                        // store object with transformation
                        scene->_lights.push_back(Light(dir, color, attenuation, /* transfstack.top() */ mat4(1.0f)));
                    }
                }
                else if (cmd == "point") {
                    validinput = readvals(s, 6, values); // Position/color for lts.
                    if (validinput) {
                        vec4 pos(values[0], values[1], values[2], 1);
                        vec3 color(values[3], values[4], values[5]);

                        // store object with transformation
                        scene->_lights.push_back(Light(pos, color, attenuation, /* transfstack.top() */ mat4(1.0f)));
                    }
                }
                else if (cmd == "attenuation") {
                    validinput = readvals(s, 3, values); // Position/color for lts.
                    if (validinput) {
                        attenuation = vec3(values[0], values[1], values[2]);
                    }
                }

                /* TODO: support sphere definition with translation, scaling and rotating */
                //if (cmd == "sphere") {
                //    //if (push_mat) {
                //    //    scene->_materials.push_back(mat);
                //    //    mat = new Material(*mat);
                //    //    push_mat = false;
                //    //}
                //    //validinput = readvals(s, 4, values); 
                //    //if (validinput) {
                //    //    // store object with material properties and transformation
                //    //    vec4 pos(values[0], values[1], values[2], 1);
                //    //    float radius = values[3];
                //    //    mat4 trans = glm::translate(mat4(1),vec3(pos));
                //    //    if (last)   // rechte matrix kommt zuerst!
                //    //        trans = trans * transfstack.top();
                //    //    else
                //    //        trans = transfstack.top() * trans;
                //    //    Sphere* s = new Sphere(trans, vec4(0.0f), radius);
                //    //    s->mat = scene->_materials.back();
                //    //    scene->_primitives.push_back(s);
                //    //}
                //}
                //
                //// transformations
                //else if (cmd == "translate") {
                //    //validinput = readvals(s,3,values); 
                //    //if (validinput) {
                //    //    mat4 trans = glm::translate(mat4(1),vec3(values[0], values[1], values[2]));
                //    //    if (last)   // rechte matrix kommt zuerst!
                //    //        transfstack.top() = trans * transfstack.top();
                //    //    else
                //    //        transfstack.top() = transfstack.top() * trans;
                //    //}
                //}
                //else if (cmd == "scale") {
                //    //validinput = readvals(s,3,values); 
                //    //if (validinput) {
                //    //    mat4 scale = glm::scale(mat4(1),vec3(values[0], values[1], values[2]));
                //    //    if (last)   // rechte matrix kommt zuerst!
                //    //        transfstack.top() = scale * transfstack.top();
                //    //    else
                //    //        transfstack.top() = transfstack.top() * scale;
                //    //}
                //}
                //else if (cmd == "rotate") {
                //    //validinput = readvals(s,4,values); 
                //    //if (validinput) {
                //    //    // rotate 0 0 1 90 
                //    //    mat4 rot = glm::rotate(mat4(1), values[3], normalize(vec3(values[0], values[1], values[2])));
                //    //    if (last)   // rechte matrix kommt zuerst!
                //    //        transfstack.top() = rot * transfstack.top();
                //    //    else
                //    //        transfstack.top() = transfstack.top() * rot;
                //    //}
                //}
                //// basic push/pop code for matrix stacks
                //else if (cmd == "pushTransform") {
                //    //transfstack.push(transfstack.top()); 
                //}
                //else if (cmd == "popTransform") {
                //    //if (transfstack.size() <= 1) {
                //    //    cerr << "Stack has no elements. Cannot Pop\n"; 
                //    //} else {
                //    //    transfstack.pop(); 
                //    //}
                //}
                //else {
                //    //cerr << "Unknown Command: " << cmd << " Skipping \n"; 
                //}
            }
            getline (in, str); 
        }
    } else {
        //cerr << "Unable to Open Input Data File " << sceneFile << "\n"; 
        throw 2; 
    }
    
    return std::move(scene);
}