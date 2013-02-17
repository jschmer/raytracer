#include <RayTracer/Scene/Primitives/Triangle.h>

#include <RayTracer/Ray.h>

// glm functions
using glm::transpose;
using glm::normalize;
using glm::dot;
using glm::cross;


Triangle::Triangle(mat4 obj2world, vec3 &f, vec3 &g, vec3 &h)
    : Primitive(obj2world)
{
    v0 = f;
    v1 = g;
    v2 = h;
    mat = nullptr;
    faceNormal = vec4(glm::cross(v1 - v0, v2 - v0), 0); 
    tranformedNormal = normalize(vec3((transpose(this->world2obj) * faceNormal)));
}

// taken from http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-9-ray-triangle-intersection/barycentric-coordinates/
float Triangle::Intersect(Ray const &ray, Intersection &Hit) {
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
void Triangle::Barycentric(vec3 phit, float &u, float &v, float &w)
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