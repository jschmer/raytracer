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

    auto x = static_cast<unsigned int>(u * this->mWidth);
    auto y = static_cast<unsigned int>(v * this->mHeight);

    // TODO: getBilinearInterpolatedColor(float x, float y);

    // image origin is top left corner
    // u, v origin is lower left corner
    unsigned int idx = (this->mHeight - y - 1)*this->mWidth + x;
    if (idx > this->mHeight * this->mWidth)
        return color3(0.0f);

    aiTexel texel = this->pcData[idx];

    color3 color;
    color.r = texel.r;
    color.g = texel.g;
    color.b = texel.b;

    // fit rgba range 0..255 into range 0..1
    for (auto i = 0u; i < 3; ++i) {
        color[i] /= 255.0f;
    }

    return color;
}

color3 Material::getTextureColor(TextureType type, float u, float v) {
    switch (type) {
    case TextureType::AMBIENT:
        return texture_ambient.getTextureColor(u, v);
        break;
    case TextureType::DIFFUSE:
        return texture_diffuse.getTextureColor(u, v);
        break;
    case TextureType::SPECULAR:
        return texture_specular.getTextureColor(u, v);
        break;
    default:
        assert(false);
        return color3(0.0f);
        break;
    }
}

Material::Material() {
    diffuse = vec3(.6f);
    shininess = 0.0f;
}

bool Material::HasTexture(TextureType type) {
    switch (type) {
    case TextureType::AMBIENT:
        return texture_ambient.pcData != nullptr;
        break;
    case TextureType::DIFFUSE:
        return texture_diffuse.pcData != nullptr;
        break;
    case TextureType::SPECULAR:
        return texture_specular.pcData != nullptr;
        break;
    default:
        return false;
        break;
    }
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
