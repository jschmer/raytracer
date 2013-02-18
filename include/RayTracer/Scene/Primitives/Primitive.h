#pragma once

#include <RayTracer/glm_includes.h>

// forward declarations
class Ray;
class Material;
class Primitive;

struct Color {
    color3 ambient;
    color3 diffuse;
    color3 specular;
    color3 emission;
    float shininess;
};

struct Intersection {
    Primitive *obj;
    vec3 hitPoint;
    vec3 normal;
    color3 color;
    Color material_color;
    float t;
};

class Primitive {
public:
    Primitive(mat4 obj2world)
        : obj2world(obj2world),
        mat()
    {
        world2obj = inverse(obj2world);
    }

    virtual float Intersect(Ray const &r, Intersection &Hit, float dist = FLT_MAX) = 0;

    // material props
    Material* mat;

    // transform matrix
    mat4 obj2world;
    mat4 world2obj;
};
