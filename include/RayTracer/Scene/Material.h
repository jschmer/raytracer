#pragma once

#include <RayTracer/glm_includes.h>
#include <assimp/scene.h> // Output data structure

class Texture : public aiTexture {
    color3 getTextureColor(float u, float v);
};

class Material {
public:
    color3 ambient;
    color3 diffuse;
    color3 specular;
    color3 emission;
    float shininess;

    Texture tex;
    bool HasTexture();

    Material();
    void setAmbient (aiColor3D &ai_ambient);
    void setDiffuse (aiColor3D &ai_diffuse);
    void setSpecular(aiColor3D &ai_specular);
    void setEmission(aiColor3D &ai_emission);
};