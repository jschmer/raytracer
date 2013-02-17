#pragma once
#include <glm/glm.hpp>
using glm::vec3;

class Ray {
public:
    Ray() {};
    Ray(vec3 pos, vec3 dir)
        : pos(pos),
        dir(dir)
    {}

    vec3 pos, dir;
};