#include <RayTracer/SceneReader/ObjParser.h>

// system stuff
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stack>

// Assimp stuff
#include <assimp/Importer.hpp>
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags

// DevIL stuff
#include <il/il.h>

// own project includes
#include <RayTracer/glm_includes.h>
#include <RayTracer/AssimpToRaytracerTypes.h>
#include <RayTracer/Scene/Scene.h>
#include <RayTracer/Scene/Camera.h>
#include <RayTracer/Scene/Material.h>
#include <RayTracer/Scene/Primitives/Triangle.h>
#include <String/StringHelper.h>

using namespace std;

std::unique_ptr<Scene> ObjParser::load() const {
    // load scene file
    Assimp::Importer loader;
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

    // Initializing il library
    ilInit();

    /*
     * Converting Assimp scene to own scene structure!
     */
    std::unique_ptr<Scene> scene(new Scene());

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

            // getting diffuse texture
            if (mat.GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiString tex_path;
                auto tex_return = mat.GetTexture(aiTextureType_DIFFUSE, 0, &tex_path); 

                if (aiReturn_SUCCESS == tex_return) {
                    // load binary texture data! with DevIL
                    ILuint texid;           /* ILuint is a 32bit unsigned integer.´Variable texid will be used to store image name. */
                    ILboolean success;      /* ILboolean is type similar to GLboolean and can equal GL_FALSE (0) or GL_TRUE (1)
                                            Variable success will be used to determine if some function returned success or failure. */
                    ilGenImages(1, &texid); /* Generation of one image name */
                    ilBindImage(texid);     /* Binding of image name */

                    std::string map_filename = scene_base_folder + tex_path.C_Str();
                    map_filename = String::replace(map_filename, "\\", "/");

                    success = ilLoadImage((const ILstring) map_filename.c_str()); /* Loading of image "image.jpg" */
                    if (success) {
                        // Convert every colour component into unsigned byte
                        // If your image contains alpha channel you can replace IL_RGB with IL_RGBA
                        success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); 
                        if (!success)
                            continue;

                        auto bpp    = ilGetInteger(IL_IMAGE_BPP);
                        auto width  = ilGetInteger(IL_IMAGE_WIDTH);
                        auto height = ilGetInteger(IL_IMAGE_HEIGHT);
                        auto pdata  = ilGetData();

                        // converting image data to aiTexels
                        auto aiTexelData = new aiTexel[width*height];
                        unsigned int dim = bpp * width * height;
                        for (auto image_idx = 0u, texel_idx = 0u; image_idx < dim; ++image_idx, ++texel_idx) {
                            auto r = pdata[image_idx];
                            auto g = pdata[++image_idx];
                            auto b = pdata[++image_idx];
                            auto a = pdata[++image_idx];

                            aiTexel texel;
                            texel.r = r;
                            texel.g = g;
                            texel.b = b;
                            texel.a = a;

                            aiTexelData[texel_idx] = texel;
                        }

                        scene_mat->tex.mHeight = height;
                        scene_mat->tex.mWidth  = width;
                        scene_mat->tex.pcData  = aiTexelData;

                        ilDeleteImages(1, &texid); /* Because we have already copied image data into texture data
                                               we can release memory used by image. */

                        printf("Using diffuse map: %s", tex_path.C_Str());
                    }
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
        scene->_lights.push_back(Light(dir, color2, attenuation, mat4(1.0f)));}

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