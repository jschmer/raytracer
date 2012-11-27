#pragma once

#include <glm\glm.hpp>

using namespace glm;

class Sphere {
public:
    Sphere(vec3 pos, float radius)
          : position(pos),
            radius(radius)
    {}

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

class Triangle {
public:
    Triangle(vec3 &f, vec3 &g, vec3 &h) {
        x = f;
        y = g;
        z = h;
        faceNormal = glm::normalize(glm::cross(z - x, y - x)); 
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