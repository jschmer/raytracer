#include <RayTracer/Scene/Primitives/AABB.h>

#include <algorithm>

bool AABB::Intersect(Ray const ray, float &hit_dist) const {
    auto tmin = (_min.x - ray.pos.x) / ray.dir.x;
    auto tmax = (_max.x - ray.pos.x) / ray.dir.x;

    if (tmin > tmax)
        std::swap(tmin, tmax);

    auto tymin = (_min.y - ray.pos.y) / ray.dir.y;
    auto tymax = (_max.y - ray.pos.y) / ray.dir.y;

    if (tymin > tymax)
        std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    auto tzmin = (_min.z - ray.pos.z) / ray.dir.z;
    auto tzmax = (_max.z - ray.pos.z) / ray.dir.z;

    if (tzmin > tzmax)
        std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    //if ((tmin > ray.tmax) || (tmax < ray.tmin))
    //    return false;
    //if (ray.tmin < tmin)
    //    ray.tmin = tmin;
    //if (ray.tmax > tmax)
    //    ray.tmax = tmax;
    return true;
}