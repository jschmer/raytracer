#include <RayTracer/Scene/Material.h>

#include <RayTracer/AssimpToRaytracerTypes.h>

color3 Texture::getTextureColor(float u, float v) {
    // coordinate origin is lower left
    // coordinates range from 0 to 1

    // u and v have to be be in the range 0..1
    u = u - floor(u);
    v = v - floor(v);

    auto x = u * this->mWidth;
    auto y = v * this->mHeight;

    typedef unsigned int uint;

    // image origin is top left corner
    // u, v origin is lower left corner (thus: height - y - 1)
    auto idx1 = static_cast<uint>((this->mHeight - floor(y) - 1)*this->mWidth + floor(x));
    auto idx2 = static_cast<uint>((this->mHeight - floor(y) - 1)*this->mWidth + floor(x) + 1);
    auto idx3 = static_cast<uint>((this->mHeight - floor(y))*this->mWidth + floor(x));
    auto idx4 = static_cast<uint>((this->mHeight - floor(y))*this->mWidth + floor(x) + 1);

    if (idx4 > this->mHeight * this->mWidth)
        return color3(0.0f);

    auto t1 = this->pcData[idx1];
    auto t2 = this->pcData[idx2];
    auto t3 = this->pcData[idx3];
    auto t4 = this->pcData[idx4];

    auto Uf = x - floor(x);
    auto Vf = y - floor(y);

    // TODO: getBilinearInterpolatedColor(float x, float y);
    auto w1 = Uf * Vf;             // linkes unten
    auto w2 = (1 - Uf) * Vf;       // rechts unten
    auto w3 = Uf * (1 - Vf);       // links oben
    auto w4 = (1 - Uf) * (1 - Vf); // rechts oben

    color3 color;
    color.r = w1*t1.r + w2*t2.r + w3*t3.r + w4*t4.r;
    color.g = w1*t1.g + w2*t2.g + w3*t3.g + w4*t4.g;
    color.b = w1*t1.b + w2*t2.b + w3*t3.b + w4*t4.b;

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
