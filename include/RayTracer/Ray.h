#pragma once

#include <RayTracer/glm_includes.h>

class Ray {
public:
    Ray() {};
    Ray(vec3 pos, vec3 dir)
        : pos(pos),
        dir(dir)
    {}

    vec3 pos, dir;
};