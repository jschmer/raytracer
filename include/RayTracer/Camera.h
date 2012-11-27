#pragma once
#include <glm\glm.hpp>
#include "Ray.h"

using namespace glm;

class Camera {
public:
    Camera(vec3 eye = vec3(), vec3 center = vec3(), vec3 up = vec3(), float fovy = 0.0F)
        : eye(eye),
        center(center),
        up(up),
        fovy(fovy)
    {}

    Ray generateRay(Sample& sample) {
        vec3 w = glm::normalize(eye-center);
        vec3 b = up;
        vec3 u = glm::normalize(glm::cross(b,w));

        Ray r;
    }

    vec3 eye;
    vec3 center;
    vec3 up;

    float fovy;
};