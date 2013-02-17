#pragma once

#include <glm/glm.hpp>

// glm types
using glm::mat4;
using glm::vec3;

// glm functions
using glm::inverse;

// forward declarations
class Ray;
class Material;
class Primitive;

struct Intersection {
    Primitive *obj;
    vec3 hitPoint;
    vec3 normal;
    vec3 color;
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

    virtual float Intersect(Ray const &r, Intersection &Hit) = 0;

    // material props
    Material* mat;

    // transform matrix
    mat4 obj2world;
    mat4 world2obj;
};
