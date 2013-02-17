#include <RayTracer/Scene/Material.h>

#include <RayTracer/AssimpToRaytracerTypes.h>

Material::Material() {
    diffuse = vec3(.6f);
    shininess = 0.0f;
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
