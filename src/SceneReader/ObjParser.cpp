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

#include <assimp/Importer.hpp>
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags

using namespace std;
using namespace glm;

std::unique_ptr<Scene> ObjParser::load() const {
    std::unique_ptr<Scene> scene(new Scene());

    vector<vec3> vertices;

    // constant, linear, quadratic attenuations 
    vec3 attenuation(1.0, 0.0, 0.0);

    {
        // fixed camera for now
        vec3 eye    = vec3(-5, -2, 10);
        vec3 center = vec3(1, 1, 1);
        vec3 up     = vec3(0, 1, 0);
        float fovy  = 30;
        scene->_camera = new Camera(eye, center, up, fovy);
    }
    {
        // point light, fourth vector member = 1
        vec4 pos(-.5, 1, 1, 1);
        vec3 color2(1, 1, 1);
        // store object with transformation
        scene->_lights.push_back(Light(pos, color2, attenuation, mat4(1.0f)));

        // directional light, fourth vector member = 0, dir is TO THE LIGHTSOURCE
        vec4 dir(-5, -2, 10, 0);
        vec3 color1(1, 1, 1);
        // store object with transformation
        scene->_lights.push_back(Light(dir, color2, attenuation, mat4(1.0f)));
    }

    // load scene file
    Assimp::Importer loader;
    const aiScene* p_scene = loader.ReadFile(sceneFile, aiProcess_Triangulate );

    if (!p_scene) {
        // ERROR while loading
        // std::cout << loader.GetErrorString() << std::endl;
        throw std::exception(loader.GetErrorString());
    }

    // convert primitives to own data structure
    for (auto mesh_idx = 0u; mesh_idx < p_scene->mNumMeshes; ++mesh_idx) {
        aiMesh& mesh = *p_scene->mMeshes[mesh_idx];
        aiMaterial& mat = *p_scene->mMaterials[mesh.mMaterialIndex];

        // getting material props
        aiColor3D ai_ambient  (0.f,0.f,0.f);
        aiColor3D ai_diffuse  (0.f,0.f,0.f);
        aiColor3D ai_specular (0.f,0.f,0.f);
        aiColor3D ai_emission (0.f,0.f,0.f);
        float ai_shininess = 0;

        mat.Get(AI_MATKEY_COLOR_AMBIENT, ai_ambient);
        mat.Get(AI_MATKEY_COLOR_DIFFUSE, ai_diffuse);
        mat.Get(AI_MATKEY_COLOR_SPECULAR, ai_specular);
        mat.Get(AI_MATKEY_COLOR_EMISSIVE, ai_emission);
        mat.Get(AI_MATKEY_SHININESS, ai_shininess);

        // convert to glm vec3
        vec3 ambient  (ai_ambient.r,  ai_ambient.g,  ai_ambient.b);
        vec3 diffuse  (ai_diffuse.r,  ai_diffuse.g,  ai_diffuse.b);
        vec3 specular (ai_specular.r, ai_specular.g, ai_specular.b);
        vec3 emission (ai_emission.r, ai_emission.g, ai_emission.b);

        if (mesh.HasFaces()) {
            // go through every face
            for (auto face_idx = 0u; face_idx < mesh.mNumFaces; ++face_idx) {
                aiFace& face = mesh.mFaces[face_idx];

                std::vector<unsigned int> vert_indices;

                // store all vertex indices
                for (auto num_face_idx = 0u; num_face_idx < face.mNumIndices; ++num_face_idx) {
                    vert_indices.push_back(face.mIndices[num_face_idx]);
                }

                // skip everything that is not a triangle
                if (3 != vert_indices.size())
                    continue;

                // construct a glm vec3 from the vert_indices
                aiVector3D v1 = mesh.mVertices[vert_indices[0]];
                aiVector3D v2 = mesh.mVertices[vert_indices[1]];
                aiVector3D v3 = mesh.mVertices[vert_indices[2]];

                vec3 v1g(v1.x, v1.y, v1.z);
                vec3 v2g(v2.x, v2.y, v2.z);
                vec3 v3g(v3.x, v3.y, v3.z);

                Triangle* t = new Triangle(mat4(1.0f), v1g, v2g, v3g);
                t->ambient  = ambient;
                t->specular = specular;
                t->emission = emission;
                t->diffuse  = diffuse;
                t->shininess = ai_shininess;
                scene->_primitives.push_back(t);
            }

        }
    }


    //// output filename
    //if (cmd == "output") {
    //    s >> scene->_outputFilename; 
    //}

    //// camera spec
    //else if (cmd == "camera") {
    //    validinput = readvals(s,10,values); // 10 values eye cen up fov
    //    if (validinput) {
    //        // camera       0 0 5 0 0 0 0 1 0 90.0
    //        // look from    0 0 5   eye
    //        // look at      0 0 0   origin
    //        // up           0 1 0   up
    //        // fovy         90.0

    //        // camera 0 -2 2 0 0 0 0 1 1 30.0
    //        vec3 eye    = vec3(values[0], values[1], values[2]);
    //        vec3 center = vec3(values[3], values[4], values[5]);
    //        vec3 up     = vec3(values[6], values[7], values[8]);
    //        float fovy  = values[9];

    //        Camera *c = new Camera(eye, center, up, fovy);
    //        delete scene->_camera;
    //        scene->_camera = c;
    //    }
    //}


    //// Process the lights, add it to database.
    //else if (cmd == "directional") {
    //    validinput = readvals(s, 6, values); // Position/color for lts.
    //    if (validinput) {
    //        vec4 dir(values[0], values[1], values[2], 0);
    //        vec3 color(values[3], values[4], values[5]);

    //        // store object with transformation
    //        scene->_lights.push_back(Light(dir, color, attenuation, mat4(1.0f)));
    //    }
    //}
    //else if (cmd == "point") {
    //    validinput = readvals(s, 6, values); // Position/color for lts.
    //    if (validinput) {
    //        vec4 pos(values[0], values[1], values[2], 1);
    //        vec3 color(values[3], values[4], values[5]);

    //        // store object with transformation
    //        scene->_lights.push_back(Light(pos, color, attenuation, mat4(1.0f)));
    //    }
    //}
    //else if (cmd == "attenuation") {
    //    validinput = readvals(s, 3, values); // Position/color for lts.
    //    if (validinput) {
    //        attenuation = vec3(values[0], values[1], values[2]);
    //    }
    //}

    //// Material Commands 
    //// Ambient, diffuse, specular, shininess properties for each object.
    //// Note that no transforms/stacks are applied to the colors. 
    //else if (cmd == "ambient") {
    //    validinput = readvals(s, 3, values); // Position/color for lts.
    //    if (validinput) {
    //        // store temporary until light/object is stored
    //        ambient = vec3(values[0], values[1], values[2]);
    //    }
    //}
    //else if (cmd == "diffuse") {
    //    validinput = readvals(s, 3, values); 
    //    if (validinput) {
    //        diffuse = vec3(values[0], values[1], values[2]);
    //    }
    //}
    //else if (cmd == "specular") {
    //    validinput = readvals(s, 3, values); 
    //    if (validinput) {
    //        specular = vec3(values[0], values[1], values[2]);
    //    }
    //}
    //else if (cmd == "emission") {
    //    validinput = readvals(s, 3, values); 
    //    if (validinput) {
    //        emission = vec3(values[0], values[1], values[2]);
    //    }
    //}
    //else if (cmd == "shininess") {
    //    validinput = readvals(s, 1, values); 
    //    if (validinput) {
    //        shininess = values[0];
    //    }
    //}

    //// Geometry

    //// vertex
    //else if (cmd == "v") {
    //    validinput = readvals(s, 3, values); 
    //    if (validinput) {
    //        vertices.push_back(vec3(values[0], values[1], values[2]));
    //    }
    //}
    //// triangle
    //else if (cmd == "f") {
    //    auto parts = String::split(s.str(), ' ');
    //    parts.erase(parts.begin()); // get rid of 'f' at the start of the cmd

    //    if (parts.size() == 3) {
    //        std::vector<vec3> verts;
    //        for (auto& part : parts) {
    //            auto single = String::split(part, '/');

    //            unsigned int vert_idx = std::stoi(single[0]);

    //            verts.push_back(vertices[vert_idx-1]);
    //        }

    //        Triangle* t = new Triangle(mat4(1.0f), verts[0], verts[1], verts[2]);
    //        t->ambient  = ambient;
    //        t->specular = specular;
    //        t->emission = emission;
    //        t->diffuse  = diffuse;
    //        t->shininess = shininess;
    //        scene->_primitives.push_back(t);
    //    }
    //}

    return scene;
}