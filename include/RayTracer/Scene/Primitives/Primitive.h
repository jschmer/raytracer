/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

#include <RayTracer/glm_includes.h>
#include <RayTracer/Scene/Primitives/AABB.h>

// forward declarations
class Ray;
class Material;
class Primitive;

struct Color {
    color3 ambient;
    color3 diffuse;
    color3 specular;
    color3 emission;
    float  shininess;
};

struct Intersection {
    bool   has_hit;
    vec3   hitPoint;
    vec3   normal;
    color3 color;
    Color  material_color;
    float  t;

    Intersection() {
        has_hit  = false;
        hitPoint = vec3(0.0f);
        normal   = vec3(0.0f);
        color    = color3(0.0f);
        t        = FLT_MAX;
    }
};

class Primitive {
public:
    Primitive(mat4 obj2world)
        : obj2world(obj2world)
    {
        world2obj = inverse(obj2world);
        mat = nullptr;
    }

    virtual float Intersect(Ray ray, Intersection &Hit, float dist = FLT_MAX) = 0;

    virtual AABB getAABB() = 0;

    // material props
    Material* mat;

    // transform matrix
    mat4 obj2world;
    mat4 world2obj;
};
