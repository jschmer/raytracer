#pragma once

#include <RayTracer/Scene/Primitives/Primitive.h>

// glm types
using glm::vec4;

// forward declarations
class Ray;

class Sphere : public Primitive {
public:
    Sphere(mat4 obj2world, vec4 pos, float radius);

    float Intersect(Ray ray, Intersection &Hit, float dist = FLT_MAX) override;

    // object parameters
    vec4 position;    // position
    float radius;
    float radius2;

    mat4 transposed_world2obj;
};