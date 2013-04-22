/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

#define GLM_SWIZZLE_XYZW 
#include <glm/glm.hpp>

// glm types
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

typedef glm::vec3 color3;
typedef glm::vec4 color4;

// glm functions
using glm::normalize;
using glm::transpose;
using glm::inverse;
using glm::cross;
using glm::dot;
using glm::radians;
namespace common = glm::common;