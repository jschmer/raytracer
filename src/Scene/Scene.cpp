#include <RayTracer/Scene/Scene.h>

#include <iostream>
#include <future>

#include <RayTracer/SceneReader/SceneReader.h>
#include <RayTracer/Scene/Camera.h>
#include <RayTracer/Scene/Material.h>
#include <RayTracer/Scene/Light.h>
#include <RayTracer/Scene/Primitives/Primitive.h>
#include <RayTracer/RenderTarget/RayTraceImage.h>

const auto aabb_dimension = 10u; 

Scene::Scene() {
    // default values
    _hasSize  = false;
    _maxdepth = 5;
    _camera = nullptr;
}

Scene::~Scene() {
    for (auto prim : _primitives)
        delete prim;

    for (auto light : _lights)
        delete light;

    for (auto mat : _materials)
        delete mat;
}

void Scene::createAABB() {
    float minx, miny, minz;
    minx = miny = minz = FLT_MAX;

    float maxx, maxy, maxz;
    maxx = maxy = maxz = FLT_MIN;

    for (auto prim : _primitives) {
        // get the primitives bounding box
        auto box = prim->getAABB();

        // min values for scene AABB
        minx = common::min(minx, box._min.x);
        miny = common::min(miny, box._min.y);
        minz = common::min(minz, box._min.z);

        // max values for scene AABB
        maxx = common::max(maxx, box._max.x);
        maxy = common::max(maxy, box._max.y);
        maxz = common::max(maxz, box._max.z);

        // store adress of primitives in overlapping boxes
    }

    _scene_aabb._min = vec3(minx, miny, minz);
    _scene_aabb._max = vec3(maxx, maxy, maxz);

    // create aabb_dimension*aabb_dimension*aabb_dimension boxes in the AABB of the scene
    float dsizeX = ((maxx - minx) / aabb_dimension) * 1.0001f; // The mul with 1.0001f accounts for a little imprecision, it makes the boxes a bit bigger than the scene AABB
    float dsizeY = ((maxy - miny) / aabb_dimension) * 1.0001f;
    float dsizeZ = ((maxz - minz) / aabb_dimension) * 1.0001f;
    
    for (float dminx = minx, dmaxx = minx + dsizeX; dminx <= maxx; ) {
        for (float dminy = miny, dmaxy = miny + dsizeY; dminy <= maxy; ) {
            for (float dminz = minz, dmaxz = minz + dsizeZ; dminz <= maxz; ) {
                // do the work
                _aabbs.push_back(AABB(vec3(dminx, dminy, dminz),
                                      vec3(dmaxx, dmaxy, dmaxz)));

                // after-run work
                dminz = dmaxz;
                dmaxz += dsizeZ;
            }

            // after-run work
            dminy = dmaxy;
            dmaxy += dsizeY;
        }

        // after-run work
        dminx  = dmaxx;
        dmaxx += dsizeX;
    }

    // fill the aabbs
    for (auto& aabb : _aabbs) {
        for (auto prim : _primitives) {
            if (aabb.Intersect(prim->getAABB()))
                aabb._primitives.push_back(prim);
        }
    }
}

bool Scene::inShadow(Ray const &ray, float t_hit = FLT_MAX) {
    Intersection hit;
    float t;

    for (std::vector<Primitive*>::iterator it = _primitives.begin(); it < _primitives.end(); ++it) {
        t = (*it)->Intersect(ray, hit);
        if (t > 0 && t < t_hit) {
            return true;
        }
    }

    return false;
}

Intersection Scene::trace(Ray const &ray, int depth) {
    // first fast test: does the ray intersect the scene AABB?
    float hist_dist;
    if (!_scene_aabb.Intersect(ray, hist_dist)) {
        return Intersection();
    }

    float t_hit = FLT_MAX;
    Intersection intersection;

    if (depth <= this->_maxdepth) {
        float t;
        Intersection hit;
        // loop through the AABBs to see if the ray intersects them
        for (auto const& aabb : _aabbs) {
            // and if it does: do the primitive intersection
            if (aabb.Intersect(ray, t)) {
                for (auto const primitive : aabb._primitives) {
                    t = primitive->Intersect(ray, hit, t_hit);
                    if (t > 0 && t < t_hit) {
                        t_hit = t;
                        intersection = hit;
                    }
                }
            }
        }
    }

    // got a hit point -> get color!
    if (intersection.has_hit) {
        intersection.color = shade(intersection, ray, depth);
    }

    return intersection;
}

vec3 Scene::shade(Intersection &Hit, Ray const &ray, int depth) {
    vec3 out_color(0.0);

    auto& material_color = Hit.material_color;

    // setting material props
    // die infos von der Intersection benutzen!
    // die verschiedenen farben sollten in der Intersect methode der Primitiven gesetzt werden
    // dort kann auch auf die texturen zugegriffen werden
    vec3& diffuse    = material_color.diffuse;
    vec3& ambient    = material_color.ambient;
    vec3& specular   = material_color.specular;
    vec3& emission   = material_color.emission;
    float& shininess = material_color.shininess;

    out_color += ambient;
    out_color += emission;

    for (auto it = _lights.begin(); it != _lights.end(); ++it) {
        auto p_light = *it;
        auto shadow_rays = p_light->getShadowRaysFrom(Hit.hitPoint);

        // test every shadow ray sample, if the ray is not in shadow, it contributes 1/number of the shadowrays 
        // to the color intensity
        float shadow_intensity = 1.0f/shadow_rays.size();
        for (auto& shadow_ray : shadow_rays) {
            // light in front of object?
            float dotP = dot(Hit.normal, shadow_ray.dir);
            if (dot(Hit.normal, shadow_ray.dir) > 0) {    
                // distance from hitpoint to light
                float max_dist = p_light->getDistanceTo(shadow_ray.pos);

                // Hitpoint ist im Schatten wenn inShadow die erste Intersection zwischen Hitpoint und Light position findet
                if (inShadow(shadow_ray, max_dist))
                    continue;       // pixel in shadow don't contribute to output color

                vec3 light_intensity = p_light->getIntensityAt(Hit.hitPoint);

                // diffuse term
                out_color += shadow_intensity * light_intensity * (diffuse * common::max(dotP, 0.0f));

                // specular term
                vec3 halfVec = normalize(shadow_ray.dir + -ray.dir);
                float halfAngle = dot(Hit.normal, halfVec);
                out_color += shadow_intensity * light_intensity * (specular * pow(common::max(halfAngle, 0.0f), shininess));
            }
        }
 }

    if (color3(0.0f) != specular) {
        // reflection
        // R = V – 2 * (V·N) * N 
        Ray reflectionRay;
        reflectionRay.dir = ray.dir - 2 * dot(ray.dir, Hit.normal) * Hit.normal;
        reflectionRay.pos = Hit.hitPoint + 0.01f*reflectionRay.dir;
    
        Intersection t = this->trace(reflectionRay, depth+1);
        if (t.has_hit)
            out_color += specular * t.color;
    }

    // TODO: Refraction

    return out_color;
}
