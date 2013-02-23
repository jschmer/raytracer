#include <RayTracer/SceneReader/TestParser.h>



#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stack>

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale

#include <RayTracer/glm_includes.h>
#include <RayTracer/Scene/Scene.h>
#include <RayTracer/Scene/Camera.h>
#include <RayTracer/Scene/Material.h>

#include <RayTracer/Scene/Primitives/Sphere.h>
#include <RayTracer/Scene/Primitives/Triangle.h>

using namespace std;

bool TestParser::readvals(stringstream &s, const int numvals, vector<float> &values) const
{
    for (int i = 0; i < numvals; i++) {
        float f;
        s >> f; 
        if (s.fail()) {
            cout << "Failed reading value " << i << " will skip\n"; 
            return false;
        }
        values.push_back(f);
    }
    return true; 
}

std::unique_ptr<Scene> TestParser::load(std::unique_ptr<Scene> scene) const {
    string str, cmd; 
    ifstream in;
    in.open(sceneFile); 
    if (in.is_open()) {
        // matrix stack to store transforms.  
        stack <mat4> transfstack; 
        transfstack.push(mat4(1.0f));  // identity

        vector<vec3> vertices;

        Material* mat = new Material;
        mat->diffuse = vec3(0);
        bool push_mat = true;

        bool last = false;   // transformation applied last

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

                // image size
                if (cmd == "maxdepth") {
                    validinput = readvals(s,1,values); 
                    if (validinput) { 
                        scene->_maxdepth = (int) values[0];
                    } 
                }

                // output filename
                else if (cmd == "output") {
                    s >> scene->_outputFilename; 
                }

                else if (cmd == "size") {
                    validinput = readvals(s, 2, values); 
                    if (validinput) {
                        scene->_size.width  = (int)values[0];
                        scene->_size.height = (int)values[1];
                        scene->_hasSize = true;
                    }
                }

                // camera spec
                else if (cmd == "camera") {
                    validinput = readvals(s,10,values); // 10 values eye cen up fov
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

                        scene->_camera.reset(new Camera(eye, center, up, fovy));
                    }
                }

                // Process the lights, add it to database.
                else if (cmd == "directional") {
                    validinput = readvals(s, 6, values); // Position/color for lts.
                    if (validinput) {
                        vec3 dir(values[0], values[1], values[2]);
                        vec3 color(values[3], values[4], values[5]);

                        // store object with transformation
                        scene->_lights.push_back(new DirectionalLight(dir, color, attenuation));
                    }
                }
                else if (cmd == "point") {
                    validinput = readvals(s, 6, values); // Position/color for lts.
                    if (validinput) {
                        vec3 pos(values[0], values[1], values[2]);
                        vec3 color(values[3], values[4], values[5]);

                        // store object with transformation
                        scene->_lights.push_back(new PointLight(pos, color, attenuation));
                    }
                }
                else if (cmd == "attenuation") {
                    validinput = readvals(s, 3, values); // Position/color for lts.
                    if (validinput) {
                        attenuation = vec3(values[0], values[1], values[2]);
                    }
                }

                // Material Commands 
                // Ambient, diffuse, specular, shininess properties for each object.
                // Note that no transforms/stacks are applied to the colors. 
                else if (cmd == "ambient") {
                    validinput = readvals(s, 3, values); // Position/color for lts.
                    if (validinput) {
                        // store temporary until light/object is stored
                        mat->ambient = vec3(values[0], values[1], values[2]);
                        push_mat = true;
                    }
                }
                else if (cmd == "diffuse") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        mat->diffuse = vec3(values[0], values[1], values[2]);
                        push_mat = true;
                    }
                }
                else if (cmd == "specular") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        mat->specular = vec3(values[0], values[1], values[2]);
                        push_mat = true;
                    }
                }
                else if (cmd == "emission") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        mat->emission = vec3(values[0], values[1], values[2]);
                        push_mat = true;
                    }
                }
                else if (cmd == "shininess") {
                    validinput = readvals(s, 1, values); 
                    if (validinput) {
                        mat->shininess = values[0];
                        push_mat = true;
                    }
                }

                // Geometry
                else if (cmd == "vertex") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        vertices.push_back(vec3(values[0], values[1], values[2]));
                    }
                }
                else if (cmd == "vertexnormal") {
                    validinput = readvals(s, 6, values); 
                    if (validinput) {

                    }
                }
                else if (cmd == "sphere") {
                    if (push_mat) {
                        scene->_materials.push_back(mat);
                        mat = new Material(*mat);
                        push_mat = false;
                    }

                    validinput = readvals(s, 4, values); 
                    if (validinput) {
                        // store object with material properties and transformation
                        vec4 pos(values[0], values[1], values[2], 1);
                        float radius = values[3];

                        mat4 trans = glm::translate(mat4(1),vec3(pos));
                        if (last)   // rechte matrix kommt zuerst!
                            trans = trans * transfstack.top();
                        else
                            trans = transfstack.top() * trans;

                        Sphere* s = new Sphere(trans, vec4(0.0f), radius);
                        s->mat = scene->_materials.back();
                        scene->_primitives.push_back(s);
                    }
                }
                else if (cmd == "tri") {
                    if (push_mat) {
                        scene->_materials.push_back(mat);
                        mat = new Material(*mat);
                        push_mat = false;
                    }

                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        vec4 v0(vertices[(unsigned int)values[0]], 1);
                        vec4 v1(vertices[(unsigned int)values[1]], 1);
                        vec4 v2(vertices[(unsigned int)values[2]], 1);

                        Triangle* t = new Triangle(transfstack.top(), vec3(v0), vec3(v1), vec3(v2));
                        t->mat = scene->_materials.back();
                        scene->_primitives.push_back(t);
                    }
                }
                else if (cmd == "trinormal") {
                    // skipping
                }

                // transformations
                else if (cmd == "translate") {
                    validinput = readvals(s,3,values); 
                    if (validinput) {
                        mat4 trans = glm::translate(mat4(1),vec3(values[0], values[1], values[2]));
                        if (last)   // rechte matrix kommt zuerst!
                            transfstack.top() = trans * transfstack.top();
                        else
                            transfstack.top() = transfstack.top() * trans;
                    }
                }
                else if (cmd == "scale") {
                    validinput = readvals(s,3,values); 
                    if (validinput) {
                        mat4 scale = glm::scale(mat4(1),vec3(values[0], values[1], values[2]));
                        if (last)   // rechte matrix kommt zuerst!
                            transfstack.top() = scale * transfstack.top();
                        else
                            transfstack.top() = transfstack.top() * scale;
                    }
                }
                else if (cmd == "rotate") {
                    validinput = readvals(s,4,values); 
                    if (validinput) {
                        // rotate 0 0 1 90 
                        mat4 rot = glm::rotate(mat4(1), values[3], normalize(vec3(values[0], values[1], values[2])));
                        if (last)   // rechte matrix kommt zuerst!
                            transfstack.top() = rot * transfstack.top();
                        else
                            transfstack.top() = transfstack.top() * rot;
                    }
                }

                else if (cmd == "maxverts") {
                    // skipping
                }

                // basic push/pop code for matrix stacks
                else if (cmd == "pushTransform") {
                    transfstack.push(transfstack.top()); 
                }
                else if (cmd == "popTransform") {
                    if (transfstack.size() <= 1) {
                        cerr << "Stack has no elements. Cannot Pop\n"; 
                    } else {
                        transfstack.pop(); 
                    }
                }
                else {
                    //cerr << "Unknown Command: " << cmd << " Skipping \n"; 
                }
            }
            getline (in, str); 
        }
    } else {
        cerr << "Unable to Open Input Data File " << sceneFile << "\n"; 
        throw 2; 
    }
    
    return std::move(scene);
}