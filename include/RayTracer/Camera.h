#pragma once
#include <glm\glm.hpp>
#include "Ray.h"

#define _USE_MATH_DEFINES
#include <math.h>

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

    void initFov(float width, float height) {
        double aspect = width/height;
        fovy = radians(fovy);
        fovx = fovy * aspect;
        tanFovx = tan(fovx/2.0f);
        tanFovy = tan(fovy/2.0f);

        halfHeight = height/2.0f;
        halfWidth = width/2.0f;
    }

    void generateRay(Sample& sample, Ray &ray) {
        // origin = eye
        // v - w = vec, der von punkt w auf punkt v zeigt
        // -> eye - center = vector, der von center auf eye zeigt
        vec3 w = normalize(eye - center);
        vec3 u = normalize(cross(up, w));
        vec3 v = cross(w, u);

        float alpha = tanFovx * ((sample.x - halfWidth)/halfWidth);
        float beta = tanFovy * ((halfHeight - sample.y)/halfHeight);

        vec3 dir = alpha*u - beta*v - w;

        ray.pos = eye;
        ray.dir = normalize(dir);
    }

    vec3 eye;
    vec3 center;
    vec3 up;

    double fovy;
    double fovx;

    double tanFovy;
    double tanFovx;

    float halfWidth;
    float halfHeight;
};