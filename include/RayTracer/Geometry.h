#pragma once

#include <glm\glm.hpp>
#include "Ray.h"

using namespace glm;

class Primitive {
public:
    Primitive()
      : ambient(0.0),
        diffuse(0.0),
        specular(0.0),
        emission(0.0) ,
        shininess(0.0f)
    {}

    virtual float Intersect(Ray& r) = 0;
    virtual vec3 Normal(vec3 hitPoint) = 0;

    // lighting
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float shininess;

    // transform matrix
    mat4 transformation;
};

class Sphere : public Primitive {
public:
    Sphere(vec3 pos, float radius)
        : position(pos),
        radius(radius)
    {}

    float Intersect(Ray &ray) {
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
            return -1.0f;

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
            return -1.0f;

        // if t0 is less than zero, the intersection point is at t1
        if (t0 < 0)
            return t1;

        // else the intersection point is at t0
        else
            return t0;
    }

    vec3 Normal(vec3 hitPoint) {
        return normalize(hitPoint - position);
    }

    // object parameters
    vec3 position;    // position
    float radius;
};

class Triangle : public Primitive {
public:
    Triangle(vec3 &f, vec3 &g, vec3 &h) {
        v0 = f;
        v1 = g;
        v2 = h;
        faceNormal = -glm::normalize(glm::cross(v2 - v0, v1 - v0)); 
    }

    float Intersect(Ray &ray) {
         vec3 v0v1 = v1 - v0;
         vec3 v0v2 = v2 - v0;
         vec3 N = cross(v0v1, v0v2);
         
         float nDotRay = dot(N, ray.dir);

         if (nDotRay == 0)
             return -1.0f; // ray parallel to triangle
         
         float d = dot(N, v0);
         float t = -(dot(N, ray.pos) + d) / nDotRay;
         
         // compute intersection point
         vec3 Phit = ray.pos + t * ray.dir;
         
         // inside-out test edge0
         vec3 v0p = Phit - v0;
         float v = dot(N, cross(v0v1, v0p));
         if (v < 0)
             return -1.0f; // P outside triangle
         
         // inside-out test edge1
         vec3 v1p = Phit - v1;
         vec3 v1v2 = v2 - v1;
         float w = dot(N, cross(v1v2, v1p));
         
         if (w < 0)
             return -1.0f; // P outside triangle
         
         // inside-out test edge2
         vec3 v2p = Phit - v2;
         vec3 v2v0 = v0 - v2;
         float u = dot(N, cross(v2v0, v2p));
         
         if (u < 0)
             return -1.0f; // P outside triangle
         
         return t;
    }

    vec3 Normal(vec3 hitPoint) {
        return normalize(faceNormal);
    }

    // object parameters
    vec3 v0, v1, v2;    // vertices
    vec3 faceNormal;
};

class Light {
public:
    Light(vec4 pos_or_dir, vec3 color, vec3 attenuation, mat4 transform)
        : pos_or_dir(pos_or_dir),
        color(color),
        attenuation(attenuation),
        transform(transform)
    {}

    vec3 LightVectorFrom(vec3 point) {
        if (pos_or_dir[3] == 1)
            // point light
            return normalize(vec3(pos_or_dir) - point);
        else
            // directional light
            return normalize(vec3(pos_or_dir));
    }

    vec3 attenuation;   // const, linear, quadratic term
    vec4 pos_or_dir;
    vec3 color;
    mat3 transform;
};