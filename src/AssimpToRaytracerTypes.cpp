/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#include <RayTracer/AssimpToRaytracerTypes.h>

vec3 ToVec3(aiColor3D &color) {
    return vec3(color.r,  color.g,  color.b);
}

vec3 ToVec3(aiVector3D &vec) {
    return vec3(vec.x,  vec.y,  vec.z);
}