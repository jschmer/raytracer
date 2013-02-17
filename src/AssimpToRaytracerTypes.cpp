#include <RayTracer/AssimpToRaytracerTypes.h>

vec3 ToVec3(aiColor3D &color) {
    return vec3(color.r,  color.g,  color.b);
}

vec3 ToVec3(aiVector3D &vec) {
    return vec3(vec.x,  vec.y,  vec.z);
}