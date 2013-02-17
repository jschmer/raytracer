#pragma once

#include <assimp/scene.h> // Output data structure
#include <glm/glm.hpp>

using namespace glm;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float shininess;

    Material();
    void setAmbient (aiColor3D &ai_ambient);
    void setDiffuse (aiColor3D &ai_diffuse);
    void setSpecular(aiColor3D &ai_specular);
    void setEmission(aiColor3D &ai_emission);
};