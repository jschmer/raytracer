#include <RayTracer/Scene/Primitives/Sphere.h>

#include <RayTracer/Ray.h>
#include <RayTracer/Scene/Material.h>

// glm functions
using glm::transpose;
using glm::normalize;
using glm::dot;

Sphere::Sphere(mat4 obj2world, vec4 pos, float radius)
    : Primitive(obj2world),
    position(pos),
    radius(radius),
    radius2(radius*radius)
{
    transposed_world2obj = transpose(this->world2obj);
}

float Sphere::Intersect(Ray ray, Intersection &Hit, float dist) {
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

    // only consider objects closer than the closest intersection until now
    if (ret > dist)
        return -1.0f;

    vec3 hitPointObjSpace = objRay.pos + ret * objRay.dir;

    // storing intersection params
    Hit.has_hit  = true;
    Hit.t        = ret;
    Hit.hitPoint = vec3(this->obj2world * vec4(hitPointObjSpace, 1));

    // calculating the normal in object space
    auto temp_normal = normalize(hitPointObjSpace - vec3(position));
    // transforming the normal
    Hit.normal = normalize(vec3(this->transposed_world2obj * vec4(temp_normal, 0)));

    // storing material color
    // TODO: fetch from texture! and barycentric coordinates from intersection point
    Hit.material_color.ambient   = mat->ambient;
    Hit.material_color.diffuse   = mat->diffuse;
    Hit.material_color.emission  = mat->emission;
    Hit.material_color.shininess = mat->shininess;
    Hit.material_color.specular  = mat->specular;

    return ret;
}