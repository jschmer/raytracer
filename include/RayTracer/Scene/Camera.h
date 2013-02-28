#pragma once
#include <RayTracer/glm_includes.h>

#include <RayTracer/RenderTarget/PNGImage.h>
#include <RayTracer/Ray.h>

#define _USE_MATH_DEFINES
#include <math.h>

class Camera {
public:
    Camera(vec3 eye, vec3 center, vec3 up, float fovy)
        : eye(eye),
        center(center),
        up(up),
        fovx(0.0),
        fovy(fovy)
    {
        // origin = eye
        // v - w = vec, der von punkt w auf punkt v zeigt
        // -> eye - center = vector, der von center auf eye zeigt
        w = normalize(eye - center);
        u = normalize(cross(up, w));
        v = cross(w, u);
    }

    void initFov(float width, float height) {
        double aspect = width/height;
        fovy    = radians(fovy);
        tanFovx = tan(fovy/2.0f)*aspect;
        tanFovy = tan(fovy/2.0f);

        halfHeight = height/2.0f;
        halfWidth = width/2.0f;
    }

    void generateRay(Sample& sample, Ray &ray) {
        float alpha = static_cast<float>(tanFovx * ((sample.x - halfWidth)/halfWidth));
        float beta = static_cast<float>(tanFovy * ((halfHeight - sample.y)/halfHeight));

        vec3 dir = alpha*u + beta*v - w;

        ray.pos = eye;
        ray.dir = normalize(dir);
    }

    vec3 eye;
    vec3 center;
    vec3 up;

    vec3 w;
    vec3 u;
    vec3 v;

    double fovy;
    double fovx;

    double tanFovy;
    double tanFovx;

    float halfWidth;
    float halfHeight;
};