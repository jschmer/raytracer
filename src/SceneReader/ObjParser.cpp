#include <RayTracer/SceneReader/ObjParser.h>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stack>

#define GLM_SWIZZLE_XYZW 
#include <glm/glm.hpp>

#include <RayTracer/Scene/Scene.h>
#include <RayTracer/Scene/Camera.h>
#include <String/StringHelper.h>

using namespace std;
using namespace glm;

bool ObjParser::readvals(stringstream &s, const int numvals, vector<float> &values) const
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

std::unique_ptr<Scene> ObjParser::load() const {
    std::unique_ptr<Scene> scene(new Scene());

    string str, cmd; 
    ifstream in;
    in.open(sceneFile); 
    if (in.is_open()) {
        vector<vec3> vertices;

        // default values
        vec3 ambient(.1, .1, .1);

        vec3 diffuse(1);
        vec3 specular(0.9);
        vec3 emission(0.0);
        float shininess = 2;

        // constant, linear, quadratic attenuations 
        vec3 attenuation(1.0, 0.0, 0.0);

        // fixed camera for now
        vec3 eye    = vec3(-30, 30, 60);
        vec3 center = vec3(0, 0, 0);
        vec3 up     = vec3(0, 1, 0);
        float fovy  = 30;
        scene->_camera = new Camera(eye, center, up, fovy);

        // point light, fourth vector member = 1
        vec4 pos(4, 4, 4, 1);
        vec3 color2(1, 0, 0);

        //// store object with transformation
        scene->_lights.push_back(Light(pos, color2, attenuation, mat4(1.0f)));

        std::getline(in, str); 
        while (in) {
            if ((str.find_first_not_of(" \t/r\n") != string::npos) && (str[0] != '#')) {
                // Ruled out comment and blank lines 

                stringstream s(str);
                s >> cmd; 

                vector<float> values;
                bool validinput; // Validity of input 

                // output filename
                if (cmd == "output") {
                    s >> scene->_outputFilename; 
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

                        Camera *c = new Camera(eye, center, up, fovy);
                        delete scene->_camera;
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
                        scene->_lights.push_back(Light(dir, color, attenuation, mat4(1.0f)));
                    }
                }
                else if (cmd == "point") {
                    validinput = readvals(s, 6, values); // Position/color for lts.
                    if (validinput) {
                        vec4 pos(values[0], values[1], values[2], 1);
                        vec3 color(values[3], values[4], values[5]);

                        // store object with transformation
                        scene->_lights.push_back(Light(pos, color, attenuation, mat4(1.0f)));
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
                        ambient = vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "diffuse") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        diffuse = vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "specular") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        specular = vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "emission") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        emission = vec3(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "shininess") {
                    validinput = readvals(s, 1, values); 
                    if (validinput) {
                        shininess = values[0];
                    }
                }

                // Geometry

                // vertex
                else if (cmd == "v") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        vertices.push_back(vec3(values[0], values[1], values[2]));
                    }
                }
                // triangle
                else if (cmd == "f") {
                    auto parts = String::split(s.str(), ' ');
                    parts.erase(parts.begin()); // get rid of 'f' at the start of the cmd

                    if (parts.size() == 3) {
                        std::vector<vec3> verts;
                        for (auto& part : parts) {
                            auto single = String::split(part, '/');

                            unsigned int vert_idx = std::stoi(single[0]);

                            verts.push_back(vertices[vert_idx-1]);
                        }

                        Triangle* t = new Triangle(mat4(1.0f), verts[0], verts[1], verts[2]);
                        t->ambient  = ambient;
                        t->specular = specular;
                        t->emission = emission;
                        t->diffuse  = diffuse;
                        t->shininess = shininess;
                        scene->_primitives.push_back(t);
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

    
    return scene;
}