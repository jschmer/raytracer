#pragma once

#include <RayTracer/glm_includes.h>
#include <RayTracer/Ray.h>

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

    bool Intersect(Ray const ray, float &hit_dist) const;

    vec3 _min;
    vec3 _max;
};