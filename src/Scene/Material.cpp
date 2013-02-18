#include <RayTracer/Scene/Material.h>

#include <RayTracer/AssimpToRaytracerTypes.h>

color3 Texture::getTextureColor(float u, float v) {
    // coordinate origin is lower left
    // coordinates range from 0 to 1

    // u and v have to be be in the range 0..1
    while (u < 0)
        ++u;
    while (u > 1)
        --u;
    while (v < 0)
        ++v;
    while (v > 1)
        --v;

    float x = u * this->mWidth;
    float y = v * this->mHeight;

    unsigned int idx = static_cast<unsigned int>(y*this->mWidth + x);
    aiTexel texel = this->pcData[idx];
    
    color3 color;
    color.r = texel.r;
    color.g = texel.g;
    color.b = texel.b;

    // fit rgba range 0..255 into range 0..1
    for (auto i = 0u; i < 3; ++i) {
        color[i] /= 255;
    }

    return color;
}

Material::Material() {
    diffuse = vec3(.6f);
    shininess = 0.0f;
}

bool Material::HasTexture() {
    return tex.pcData != nullptr;
}

void Material::setAmbient(aiColor3D &ai_ambient) {
    ambient = ToVec3(ai_ambient);
}

void Material::setDiffuse(aiColor3D &ai_diffuse) {
    diffuse = ToVec3(ai_diffuse);
}

void Material::setSpecular(aiColor3D &ai_specular) {
    specular = ToVec3(ai_specular);
}

void Material::setEmission(aiColor3D &ai_emission) {
    emission = ToVec3(ai_emission);
}
