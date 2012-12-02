#include "..\include\RayTracer\SceneParser.h"

#include <RayTracer\SceneParser.h>
#include <RayTracer\Scene.h>
#include <RayTracer\Transform.h>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stack>

#include <glm\glm.hpp>

using namespace std;
using namespace glm;

bool SceneParser::readvals(stringstream &s, const int numvals, vector<float> &values) 
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

Scene* SceneParser::load(){
    Scene *scene = new Scene();

    string str, cmd; 
    ifstream in;
    in.open(sceneFile); 
    if (in.is_open()) {
        // matrix stack to store transforms.  
        stack <mat4> transfstack; 
        transfstack.push(mat4(1.0f));  // identity

        vector<vec3> vertices;

        vec3 ambient(0.2f, 0.2f, 0.2f);

        vec3 diffuse(0.0);
        vec3 specular(0.0);
        vec3 emission(0.0);
        float shininess = 0;

        // constant, linear, quadratic attenuations 
        vec3 attenuation(1.0, 0.0, 0.0);

        std::getline(in, str); 
        while (in) {
            if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
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

                else if (cmd == "size") {
                    validinput = readvals(s, 2, values); 
                    if (validinput) {
                        scene->_size.width  = (int)values[0];
                        scene->_size.height = (int)values[1];
                    }
                }

                // output filename
                else if (cmd == "output") {
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
                        vec3 up     = Transform::upvector(vec3(values[6], values[7], values[8]), eye);
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
                        scene->_lights.push_back(Light(dir, color, attenuation, transfstack.top()));
                    }
                }
                else if (cmd == "point") {
                    validinput = readvals(s, 6, values); // Position/color for lts.
                    if (validinput) {
                        vec4 pos(values[0], values[1], values[2], 1);
                        vec3 color(values[3], values[4], values[5]);

                        // store object with transformation
                        scene->_lights.push_back(Light(pos, color, attenuation, transfstack.top()));
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
                } else if (cmd == "specular") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        specular = vec3(values[0], values[1], values[2]);
                    }
                } else if (cmd == "emission") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        emission = vec3(values[0], values[1], values[2]);
                    }
                } else if (cmd == "shininess") {
                    validinput = readvals(s, 1, values); 
                    if (validinput) {
                        shininess = values[0];
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
                    validinput = readvals(s, 4, values); 
                    if (validinput) {
                        // store object with material properties and transformation
                        Sphere* s = new Sphere(vec3(values[0], values[1], values[2]), values[3]);
                        s->ambient  = ambient;
                        s->specular = specular;
                        s->emission = emission;
                        s->diffuse  = diffuse;
                        s->shininess = shininess;
                        s->transformation = transfstack.top();
                        scene->_primitives.push_back(s);
                    }
                }
                else if (cmd == "tri") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        Triangle* t = new Triangle( vertices[(unsigned int)values[0]],
                                    vertices[(unsigned int)values[1]],
                                    vertices[(unsigned int)values[2]]);
                        t->ambient  = ambient;
                        t->specular = specular;
                        t->emission = emission;
                        t->diffuse  = diffuse;
                        t->shininess = shininess;
                        t->transformation = transfstack.top();
                        scene->_primitives.push_back(t);
                    }
                }

                else if (cmd == "trinormal") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {

                    }
                }

                // transformations
                else if (cmd == "translate") {
                    validinput = readvals(s,3,values); 
                    if (validinput) {
                        transfstack.top() = transfstack.top()*Transform::translate(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "scale") {
                    validinput = readvals(s,3,values); 
                    if (validinput) {
                        transfstack.top() = transfstack.top()*Transform::scale(values[0], values[1], values[2]);
                    }
                }
                else if (cmd == "rotate") {
                    validinput = readvals(s,4,values); 
                    if (validinput) {
                        // rotate 0 0 1 90 
                        mat4 rotMatrix(Transform::rotate(values[3], vec3(values[0], values[1], values[2])));
                        transfstack.top() = transfstack.top()*rotMatrix;
                    }
                }

                // basic push/pop code for matrix stacks
                else if (cmd == "pushTransform") {
                    transfstack.push(transfstack.top()); 
                } else if (cmd == "popTransform") {
                    if (transfstack.size() <= 1) {
                        cerr << "Stack has no elements. Cannot Pop\n"; 
                    } else {
                        transfstack.pop(); 
                    }
                }
                else {
                    cerr << "Unknown Command: " << cmd << " Skipping \n"; 
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