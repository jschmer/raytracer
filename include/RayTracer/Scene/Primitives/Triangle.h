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
    float Intersect(Ray ray, Intersection &Hit, float dist = FLT_MAX) override;
    AABB getAABB() override;

    // Compute barycentric coordinates (u, v, w) for
    // point p with respect to triangle (a, b, c)
    void Barycentric(vec3 phit, float &u, float &v, float &w);
    bool HasTextureCoords();

    // object parameters
    vec3 v0,  v1,  v2;    // vertices
    vec3 vt0, vt1, vt2;   // texture vertices
    bool hasTextureCoords;

    // cached triangle data
    vec4 vec4_faceNormal;
    vec3 vec3_faceNormal;
    vec3 transformedNormal;

    vec3 v0v1;    // v0 nach v1!
    vec3 v0v2;
    vec3 v1v2;
    vec3 v2v0;

    float normal_len;
    float d;

    bool has_transformation_matrix;  ///< true if the transformation matrix is anything other than the identity
};