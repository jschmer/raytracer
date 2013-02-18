#pragma once

#include <RayTracer/Scene/Primitives/Primitive.h>

// glm types
using glm::vec4;

// forward declarations
class Ray;


class Triangle : public Primitive {
public:
    Triangle(mat4 obj2world, vec3 &f, vec3 &g, vec3 &h);

    // taken from http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-9-ray-triangle-intersection/barycentric-coordinates/
    float Intersect(Ray const &ray, Intersection &Hit, float dist = FLT_MAX) override;

    // Compute barycentric coordinates (u, v, w) for
    // point p with respect to triangle (a, b, c)
    void Barycentric(vec3 phit, float &u, float &v, float &w);

    bool HasTextureCoords();

    // object parameters
    vec3 v0, v1, v2;    // vertices
    vec3 vt0, vt1, vt2; // texture vertices
    bool hasTextureCoords;
    vec4 faceNormal;
    vec3 tranformedNormal;
};