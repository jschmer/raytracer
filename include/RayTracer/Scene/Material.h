#pragma once

#include <RayTracer/glm_includes.h>
#include <assimp/scene.h> // Output data structure

class Texture : public aiTexture {
public:
    color3 getTextureColor(float u, float v);
};

enum TextureType : char {
    AMBIENT,
    DIFFUSE,
    SPECULAR
};

class Material {
public:
    color3 ambient;
    color3 diffuse;
    color3 specular;
    color3 emission;
    float shininess;

    Texture texture_ambient;
    Texture texture_diffuse;
    Texture texture_specular;
    bool HasTexture(TextureType type);

    Material();
    void setAmbient (aiColor3D &ai_ambient);
    void setDiffuse (aiColor3D &ai_diffuse);
    void setSpecular(aiColor3D &ai_specular);
    void setEmission(aiColor3D &ai_emission);
};