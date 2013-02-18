#pragma once

#include <assimp/scene.h> // assimp data types
#include <RayTracer/glm_includes.h>

color3 ToVec3(aiColor3D &color);
vec3 ToVec3(aiVector3D &vec);