#pragma once
#include <RayTracer/glm_includes.h>

#include <RayTracer/RenderTarget/IRenderTarget.h>
#include <RayTracer/Ray.h>

enum Direction {
    ROTATE_LEFT,
    ROTATE_RIGHT,
    ROTATE_UP,
    ROTATE_DOWN,
    STRAFE_LEFT,
    STRAFE_RIGHT,
    STRAFE_UP,
    STRAFE_DOWN,
    CLOSER,
    FARTHER
};

class Camera {
public:
    Camera(vec3 eye, vec3 center, vec3 up, float fovy);

    void initFov(float width, float height);

    void generateRay(Sample& sample, Ray &ray);

    void move(Direction dir, float angle);

    void moveAroundCrystalBall(Direction dir, float angle);

private:
    vec3 _eye;
    vec3 _center;
    vec3 _up;
    vec3 _looking_dir;

    vec3 _w;
    vec3 _u;
    vec3 _v;

    double _fovy;
    double _fovx;

    double _tanFovy;
    double _tanFovx;

    float _halfWidth;
    float _halfHeight;
};