#include <RayTracer/Scene/Primitives/AABB.h>

#include <algorithm>

bool AABB::Intersect(Ray const &ray, float &hit_dist) const {
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

    auto tzmax = (_min.z - ray.pos.z) / ray.dir.z;
    auto tzmin = (_max.z - ray.pos.z) / ray.dir.z;

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

bool AABB::Intersect(AABB const &other) const {
    for (auto i = 0u; i < 3; ++i)
        if (_min[i] > other._max[i] || other._min[i] > _max[i])
            return false;
    return true;
}

bool AABB::isEmpty() const {
    return _primitives.size() == 0;
}