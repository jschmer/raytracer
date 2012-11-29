#pragma once

#include <glm\glm.hpp>
#include "Ray.h"

using namespace glm;

class Primitive {
public:
    virtual float Intersect(Ray& r, vec3 &color) = 0;
};

class Sphere : public Primitive {
public:
    Sphere(vec3 pos, float radius)
        : position(pos),
        radius(radius)
    {}

    float Intersect(Ray &ray, vec3 &color) {
        //Compute A, B and C coefficients
        float a = dot(ray.dir, ray.dir);
        vec3 center_to_camera = ray.pos - position;
        float b = 2*dot(ray.dir, center_to_camera);
        float c = dot(center_to_camera, center_to_camera) - (radius * radius);

        //Find discriminant
        float disc = b * b - 4 * a * c;

        // if discriminant is negative there are no real roots, so return 
        // false as ray misses sphere
        if (disc < 0)
            return false;

        color = vec3(1, 0, 0);

        // compute q as described above
        float distSqrt = sqrtf(disc);
        float q;
        if (b < 0)
            q = (-b - distSqrt)/2.0f;
        else
            q = (-b + distSqrt)/2.0f;

        // compute t0 and t1
        float t0 = q / a;
        float t1 = c / q;

        // make sure t0 is smaller than t1
        if (t0 > t1)
        {
            // if t0 is bigger than t1 swap them around
            float temp = t0;
            t0 = t1;
            t1 = temp;
        }

        // if t1 is less than zero, the object is in the ray's negative direction
        // and consequently the ray misses the sphere
        if (t1 < 0)
            return false;

        // if t0 is less than zero, the intersection point is at t1
        if (t0 < 0)
            return t1;

        // else the intersection point is at t0
        else
            return t0;
    }

    // object parameters
    vec3 position;    // position
    float radius;

    // lighting
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;

    // transform matrix
    mat4 transformation;
};

class Triangle : public Primitive {
public:
    Triangle(vec3 &f, vec3 &g, vec3 &h) {
        x = f;
        y = g;
        z = h;
        faceNormal = glm::normalize(glm::cross(z - x, y - x)); 
    }

    float Intersect(Ray &ray, vec3 &color) {
        return false;
    }

    // object parameters
    vec3 x, y, z;    // vertices
    vec3 faceNormal;

    // lighting
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;

    // transform matrix
    mat4 transformation;
};

class Light {
public:
    Light(vec4 pos_or_dir, vec3 color, vec3 attenuation, mat4 transform)
        : pos_or_dir(pos_or_dir),
        color(color),
        attenuation(attenuation),
        transform(transform)
    {}

    vec3 attenuation;   // const, linear, quadratic term
    vec4 pos_or_dir;
    vec3 color;
    mat3 transform;
};