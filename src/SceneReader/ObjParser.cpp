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

#include <RayTracer/AssimpToRaytracerTypes.h>

using namespace std;
using namespace glm;

std::unique_ptr<Scene> ObjParser::load() const {
    // load scene file
    Assimp::Importer loader;
    const aiScene* ai_scene = loader.ReadFile(sceneFile, aiProcess_Triangulate );

    if (!ai_scene) {
        // ERROR while loading
        throw std::exception(loader.GetErrorString());
    }

    /*
     * Converting Assimp scene to own scene structure!
     */
    std::unique_ptr<Scene> scene(new Scene());

    /*
     * Adding materials
     */
    if (ai_scene->HasMaterials()) {
        for (auto mat_idx = 0u; mat_idx < ai_scene->mNumMaterials; ++mat_idx) {
            aiMaterial& mat = *ai_scene->mMaterials[mat_idx];

            // material props vars
            aiColor3D ai_ambient  (0.f,0.f,0.f);
            aiColor3D ai_diffuse  (0.f,0.f,0.f);
            aiColor3D ai_specular (0.f,0.f,0.f);
            aiColor3D ai_emission (0.f,0.f,0.f);
            float ai_shininess = 0;

            // getting material props
            mat.Get(AI_MATKEY_COLOR_AMBIENT, ai_ambient);
            mat.Get(AI_MATKEY_COLOR_DIFFUSE, ai_diffuse);
            mat.Get(AI_MATKEY_COLOR_SPECULAR, ai_specular);
            mat.Get(AI_MATKEY_COLOR_EMISSIVE, ai_emission);
            mat.Get(AI_MATKEY_SHININESS, ai_shininess);

            // storing own material in scene
            Material scene_mat;
            scene_mat.setAmbient (ai_ambient);
            scene_mat.setDiffuse (ai_diffuse);
            scene_mat.setSpecular(ai_specular);
            scene_mat.setEmission(ai_emission);

            scene->_materials.push_back(scene_mat);

            // getting diffuse texture
            if (mat.GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiString tex_path;
                auto tex_return = mat.GetTexture(aiTextureType_DIFFUSE, 0, &tex_path); 

                // TODO: load binary texture data!
                aiTexture tex;

                printf(tex_path.C_Str());
            }
        }
    }

    /*
     * Adding geometry / primitives
     */
    for (auto mesh_idx = 0u; mesh_idx < ai_scene->mNumMeshes; ++mesh_idx) {
        aiMesh& mesh = *ai_scene->mMeshes[mesh_idx];
        
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

                Triangle* t = new Triangle(mat4(1.0f), ToVec3(v1), ToVec3(v2), ToVec3(v3));
                t->mat = scene->_materials[mesh.mMaterialIndex];
                scene->_primitives.push_back(t);
            }

        }
    }

    {
        /*
         * Adding lights, TODO!
         */
        // constant, linear, quadratic attenuations 
        vec3 attenuation(1.0, 0.0, 0.0);

        // point light, fourth vector member = 1
        vec4 pos(-.5, 1, 1, 1);
        vec3 color2(1, 1, 1);
        // store object with transformation
        scene->_lights.push_back(Light(pos, color2, attenuation, mat4(1.0f)));

        // directional light, fourth vector member = 0, dir is TO THE LIGHTSOURCE
        vec4 dir(-5, -2, 10, 0);
        vec3 color1(.9);
        // store object with transformation
        scene->_lights.push_back(Light(dir, color2, attenuation, mat4(1.0f)));
    }

    {
        /*
         * Adding camera, TODO!
         */
        vec3 eye    = vec3(-5, -2, 10);
        vec3 center = vec3(1, 1, 1);
        vec3 up     = vec3(0, 1, 0);
        float fovy  = 30;

        delete scene->_camera;
        scene->_camera = new Camera(eye, center, up, fovy);
    }

    return scene;
}