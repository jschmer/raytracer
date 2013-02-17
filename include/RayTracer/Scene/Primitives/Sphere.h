#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include <RayTracer/Ray.h>
#include <RayTracer/Scene/Material.h>
#include <RayTracer/Scene/Primitives/Primitive.h>

class Sphere : public Primitive {
public:
    Sphere(mat4 obj2world, vec4 pos, float radius);

    float Intersect(Ray const &ray, Intersection &Hit) override;

    // object parameters
    vec4 position;    // position
    float radius;
    float radius2;

    mat4 transposed_world2obj;
};