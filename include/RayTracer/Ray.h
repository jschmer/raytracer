/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

#include <RayTracer/glm_includes.h>

class Ray {
public:
    Ray() {};
    Ray(vec3 pos, vec3 dir)
        : pos(pos),
        dir(dir)
    {}
    // move constructor
    Ray(Ray&& other)
        : pos(other.pos)
        , dir(other.dir)
    {}

    vec3 pos, dir;
};