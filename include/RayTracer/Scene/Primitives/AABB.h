/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

#include <vector>

#include <RayTracer/glm_includes.h>
#include <RayTracer/Ray.h>

class Primitive;

class AABB {
public:
    AABB() 
        : _min(),
          _max()
    {}
    AABB(vec3 min, vec3 max)
        : _min(min),
          _max(max)
    {}

    bool Intersect(Ray const &ray, float &hit_dist) const;
    bool Intersect(AABB const &other) const;
    bool isEmpty() const;

    vec3 _min;
    vec3 _max;
    std::vector<Primitive*> _primitives;
};