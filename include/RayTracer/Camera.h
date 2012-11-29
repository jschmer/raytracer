#pragma once
#include <glm\glm.hpp>
#include "Ray.h"

using namespace glm;

class Camera {
public:
    Camera(vec3 eye, vec3 center, vec3 up, float fovy)
        : eye(eye),
        center(center),
        up(up),
        fovx(0.0),
        fovy(fovy)
    {}

    Ray generateRay(Sample& sample) {
        // origin = eye


        // vec v - vec w = vec, der von punkt w auf punkt v zeigt
    }

    vec3 eye;
    vec3 center;
    vec3 up;

    float fovy;
    float fovx;

    float width;
    float height;
};