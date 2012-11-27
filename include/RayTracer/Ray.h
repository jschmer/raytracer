#pragma once
#include <glm\glm.hpp>

using namespace glm;

class Ray {
public:
    Ray(vec3 pos = vec3(), vec3 dir = vec3(), float t_min = 1, float t_max = FLT_MAX)
        : pos(pos),
        dir(dir),
        t_min(t_min),
        t_max(t_max)
    {}

    vec3 pos, dir;
    float t_min, t_max;
};