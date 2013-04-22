/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

#include <assimp/scene.h> // assimp data types
#include <RayTracer/glm_includes.h>

color3 ToVec3(aiColor3D &color);
vec3 ToVec3(aiVector3D &vec);