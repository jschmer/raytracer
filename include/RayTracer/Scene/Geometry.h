#pragma once

#include <glm/glm.hpp>
#include <RayTracer/Ray.h>
#include <RayTracer/Scene/Scene.h>
#include <RayTracer/Scene/Material.h>

using namespace glm;

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
    Material mat;

    // transform matrix
    mat4 obj2world;
    mat4 world2obj;
};

class Sphere : public Primitive {
public:
    Sphere(mat4 obj2world, vec4 pos, float radius)
        : Primitive(obj2world),
        position(pos),
        radius(radius),
        radius2(radius*radius)
    {
        transposed_world2obj = transpose(this->world2obj);
    }

    float Intersect(Ray const &ray, Intersection &Hit) {
        // transforming ray to object space
        Ray objRay;
        objRay.pos = vec3(this->world2obj * vec4(ray.pos, 1));
        objRay.dir = vec3(this->world2obj * vec4(ray.dir, 0));

        //Compute A, B and C coefficients
        float a = dot(objRay.dir, objRay.dir);
        float b = 2*dot(objRay.dir, objRay.pos);
        float c = dot(objRay.pos, objRay.pos) - radius2;

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

        float ret;
        // if t0 is less than zero, the intersection point is at t1
        if (t0 < 0)
            ret = t1;

        // else the intersection point is at t0
        else
            ret = t0;

        vec3 hitPointObjSpace = objRay.pos + ret * objRay.dir;

        Hit.obj = this;
        Hit.t = ret;
        Hit.hitPoint = vec3(this->obj2world * vec4(hitPointObjSpace, 1));

        // calculating the normal in object space
        Hit.normal = normalize(hitPointObjSpace - vec3(position));
        // transforming the normal
        Hit.normal = normalize(vec3(this->transposed_world2obj * vec4(Hit.normal, 0)));
        
        return ret;
    }

    // object parameters
    vec4 position;    // position
    float radius;
    float radius2;

    mat4 transposed_world2obj;
};

class Triangle : public Primitive {
public:
    Triangle(mat4 obj2world, vec3 &f, vec3 &g, vec3 &h)
        : Primitive(obj2world)
    {
        v0 = f;
        v1 = g;
        v2 = h;
        faceNormal = vec4(glm::cross(v1 - v0, v2 - v0), 0); 
        tranformedNormal = normalize(vec3((transpose(this->world2obj) * faceNormal)));
    }

    // taken from http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-9-ray-triangle-intersection/barycentric-coordinates/
    float Intersect(Ray const &ray, Intersection &Hit) {
        // transforming ray to object space
        Ray r;
        r.pos = vec3(this->world2obj * vec4(ray.pos, 1));
        r.dir = vec3(this->world2obj * vec4(ray.dir, 0));

        vec3 v0v1 = v1 - v0;    // v0 nach v1!
        vec3 v0v2 = v2 - v0;
        vec3 N = vec3(faceNormal); //cross(v0v1, v0v2);

        float nDotRay = dot(N, r.dir);

        if (nDotRay == 0)
            return -1.0f; // ray parallel to triangle

        float d = dot(N, v0);
        float t = -(dot(N, r.pos) - d) / nDotRay;

        // compute intersection point
        vec3 Phit = r.pos + t * r.dir;

        vec3 v0p = Phit - v0;

        float bu, bv, bw;
        Barycentric(Phit, bu, bv, bw);

        // inside-out test edge0
        float v = dot(N, cross(v0v1, v0p));
        if (v < 0)
            return -1.0f; // P outside triangle

        // inside-out test edge1
        float w = dot(N, cross(v2 - v1, Phit - v1));
        if (w < 0)
            return -1.0f; // P outside triangle

        // inside-out test edge2
        float u = dot(N, cross(v0 - v2, Phit - v2));
        if (u < 0)
            return -1.0f; // P outside triangle

        // calculate barycentric coords
        auto nlen2 = dot(N, N);
        u = u / nlen2;
        v = v / nlen2;
        w = 1 - u - v;

        // interpolate texture coords
        // p = w
        // p - p0 = (p2 - p1, p1 - p0)()
        // solve: 

        Hit.obj = this;
        Hit.t = t;
        vec3 hitPoint = Phit;
        Hit.hitPoint = vec3(this->obj2world * vec4(hitPoint, 1));
        Hit.normal = this->tranformedNormal;
        return t;
    }

    // Compute barycentric coordinates (u, v, w) for
    // point p with respect to triangle (a, b, c)
    void Barycentric(vec3 phit, float &u, float &v, float &w)
    {
        vec3 v0v1 = v1 - v0;
        vec3 v0v2 = v2 - v0;
        vec3 v0p  = phit - v0;
        float d00 = dot(v0v1, v0v1);
        float d01 = dot(v0v1, v0v2);
        float d11 = dot(v0v2, v0v2);
        float d20 = dot(v0p, v0v1);
        float d21 = dot(v0p, v0v2);
        float denom = d00 * d11 - d01 * d01;
        v = (d11 * d20 - d01 * d21) / denom;
        w = (d00 * d21 - d01 * d20) / denom;
        u = 1.0f - v - w;
    }

    // object parameters
    vec3 v0, v1, v2;    // vertices
    vec4 faceNormal;
    vec3 tranformedNormal;
};

class Light {
public:
    Light(vec4 pos_or_dir, vec3 color, vec3 attenuation, mat4 transform)
        : pos_or_dir(pos_or_dir),
        color(color),
        attenuation(attenuation),
        transform(transform)
    {
        normalized_dir = normalize(vec3(pos_or_dir));
    }

    vec3 LightVectorFrom(vec3 const &point) const {
        if (pos_or_dir[3] == 1)
            // point light
            return normalize(vec3(pos_or_dir) - point);
        else
            // directional light, pos_or_dir is the light vector TO the light source
            return this->normalized_dir;
    }

    vec3 attenuation;   // const, linear, quadratic term

    vec4 pos_or_dir;
    vec3 normalized_dir;
    vec3 color;

    mat3 transform;
};