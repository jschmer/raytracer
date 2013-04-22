/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#include <RayTracer/SceneReader/AssimpParser.h>

// system stuff
#include <string>

// Assimp stuff
#include <assimp/Importer.hpp>
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags

// own project includes
#include <RayTracer/glm_includes.h>
#include <RayTracer/AssimpToRaytracerTypes.h>
#include <RayTracer/Scene/Scene.h>
#include <RayTracer/Scene/Camera.h>
#include <RayTracer/Scene/Material.h>
#include <RayTracer/Scene/Primitives/Triangle.h>
#include <RayTracer/SceneReader/TextureLoader.h>
#include <String/StringHelper.h>

using namespace std;

std::unique_ptr<Scene> AssimpParser::load(std::unique_ptr<Scene> scene) const {
    // load scene file
    Assimp::Importer loader;

    /* Available flag for assimp importer
    aiProcess_CalcTangentSpace;
    aiProcess_ConvertToLeftHanded;
    aiProcess_Debone;
    aiProcess_FindDegenerates;
    aiProcess_FindInstances;
    aiProcess_FindInvalidData;
    aiProcess_FixInfacingNormals;
    aiProcess_FlipUVs;
    aiProcess_FlipWindingOrder;
    aiProcess_GenNormals;
    aiProcess_GenSmoothNormals;
    aiProcess_GenUVCoords;
    aiProcess_ImproveCacheLocality;
    aiProcess_JoinIdenticalVertices;
    aiProcess_LimitBoneWeights;
    aiProcess_MakeLeftHanded;
    aiProcess_OptimizeGraph;
    aiProcess_OptimizeMeshes;
    aiProcess_PreTransformVertices;
    aiProcess_RemoveComponent;
    aiProcess_RemoveRedundantMaterials;
    aiProcess_SortByPType;
    aiProcess_SplitByBoneCount;
    aiProcess_SplitLargeMeshes;
    aiProcess_TransformUVCoords;
    aiProcess_Triangulate;
    aiProcess_ValidateDataStructure;
    */

    const aiScene* ai_scene = loader.ReadFile(sceneFile, aiProcess_Triangulate );

    // getting the folder the sceneFile lives in
    std::string scene_base_folder;
    auto pos = sceneFile.rfind('/');
    if (std::string::npos != pos)
        scene_base_folder = sceneFile.substr(0, pos + 1);
    else
        scene_base_folder = "./";

    if (!ai_scene) {
        // ERROR while loading
        throw std::exception(loader.GetErrorString());
    }

    /*
     * Looping through Nodes...
     */
    for (auto i = 0u; i < ai_scene->mRootNode->mNumChildren; ++i) {
        aiNode& node = *ai_scene->mRootNode->mChildren[i];
    }

    /*
     * Adding materials
     */
    if (ai_scene->HasMaterials()) {
        for (auto mat_idx = 0u; mat_idx < ai_scene->mNumMaterials; ++mat_idx) {
            const aiMaterial& mat = *ai_scene->mMaterials[mat_idx];

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
            auto scene_mat = new Material;
            scene_mat->setAmbient (ai_ambient);
            scene_mat->setDiffuse (ai_diffuse);
            scene_mat->setSpecular(ai_specular);
            scene_mat->setEmission(ai_emission);

            // getting ambient texture
            if (mat.GetTextureCount(aiTextureType_AMBIENT) > 0) {
                aiString tex_path;
                auto tex_return = mat.GetTexture(aiTextureType_AMBIENT, 0, &tex_path); 

                if (aiReturn_SUCCESS == tex_return) {
                    std::string map_filename = scene_base_folder + tex_path.C_Str();
                    map_filename = String::replace(map_filename, "\\", "/");

                    unsigned int width, height;
                    auto texel_data = loadTexture(map_filename, width, height);

                    scene_mat->texture_ambient.mHeight = height;
                    scene_mat->texture_ambient.mWidth  = width;
                    scene_mat->texture_ambient.pcData  = texel_data;

                    printf("Using ambient map: %s\n", tex_path.C_Str());
                }
            }

            // getting diffuse texture
            if (mat.GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiString tex_path;
                auto tex_return = mat.GetTexture(aiTextureType_DIFFUSE, 0, &tex_path); 

                if (aiReturn_SUCCESS == tex_return) {
                    std::string map_filename = scene_base_folder + tex_path.C_Str();
                    map_filename = String::replace(map_filename, "\\", "/");

                    unsigned int width, height;
                    auto texel_data = loadTexture(map_filename, width, height);

                    scene_mat->texture_diffuse.mHeight = height;
                    scene_mat->texture_diffuse.mWidth  = width;
                    scene_mat->texture_diffuse.pcData  = texel_data;

                    printf("Using diffuse map: %s\n", tex_path.C_Str());
                }
            }

            // getting specular texture
            if (mat.GetTextureCount(aiTextureType_SPECULAR) > 0) {
                aiString tex_path;
                auto tex_return = mat.GetTexture(aiTextureType_SPECULAR, 0, &tex_path); 

                if (aiReturn_SUCCESS == tex_return) {
                    std::string map_filename = scene_base_folder + tex_path.C_Str();
                    map_filename = String::replace(map_filename, "\\", "/");

                    unsigned int width, height;
                    auto texel_data = loadTexture(map_filename, width, height);

                    scene_mat->texture_specular.mHeight = height;
                    scene_mat->texture_specular.mWidth  = width;
                    scene_mat->texture_specular.pcData  = texel_data;

                    printf("Using specular map: %s\n", tex_path.C_Str());
                }
            }

            scene->_materials.push_back(scene_mat);
        }
    }

    /*
     * Adding geometry / primitives
     */
    for (auto mesh_idx = 0u; mesh_idx < ai_scene->mNumMeshes; ++mesh_idx) {
        const aiMesh& mesh = *ai_scene->mMeshes[mesh_idx];

        if (mesh.HasFaces()) {
            // go through every face
            for (auto face_idx = 0u; face_idx < mesh.mNumFaces; ++face_idx) {
                const aiFace& face = mesh.mFaces[face_idx];

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
                if (mesh.GetNumUVChannels()) {
                    t->vt0 = ToVec3(mesh.mTextureCoords[0][vert_indices[0]]);
                    t->vt1 = ToVec3(mesh.mTextureCoords[0][vert_indices[1]]);
                    t->vt2 = ToVec3(mesh.mTextureCoords[0][vert_indices[2]]);
                    t->hasTextureCoords = true;
                }
                t->mat = scene->_materials[mesh.mMaterialIndex];
                scene->_primitives.push_back(t);
            }
        }
    }

    // Camera and Lights can be specified in a .raytrace file!

    return std::move(scene);
}