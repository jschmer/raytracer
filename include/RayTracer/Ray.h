#pragma once
#include <glm\glm.hpp>

using namespace glm;

class Ray {
public:
    Ray() {};
    Ray(vec3 pos, vec3 dir, float t_min = 1, float t_max = FLT_MAX)
        : pos(pos),
        dir(dir),
        t_min(t_min),
        t_max(t_max)
    {}

    vec3 pos, dir;
    float t_min, t_max;
};