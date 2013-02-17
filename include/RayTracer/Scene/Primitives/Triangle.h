#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include <RayTracer/Ray.h>
#include <RayTracer/Scene/Scene.h>
#include <RayTracer/Scene/Material.h>
#include <RayTracer/Scene/Primitives/Primitive.h>

class Triangle : public Primitive {
public:
    Triangle(mat4 obj2world, vec3 &f, vec3 &g, vec3 &h);

    // taken from http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-9-ray-triangle-intersection/barycentric-coordinates/
    float Intersect(Ray const &ray, Intersection &Hit) override;

    // Compute barycentric coordinates (u, v, w) for
    // point p with respect to triangle (a, b, c)
    void Barycentric(vec3 phit, float &u, float &v, float &w);

    // object parameters
    vec3 v0, v1, v2;    // vertices
    vec4 faceNormal;
    vec3 tranformedNormal;
};