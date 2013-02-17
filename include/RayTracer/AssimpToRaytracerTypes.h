#pragma once

#include <assimp/scene.h> // assimp data types
#include <glm/glm.hpp>
using namespace glm;

vec3 ToVec3(aiColor3D &color);
vec3 ToVec3(aiVector3D &vec);