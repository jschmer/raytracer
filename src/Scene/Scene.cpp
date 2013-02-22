#include <RayTracer/Scene/Scene.h>

#include <iostream>
#include <future>

#include <RayTracer/SceneReader/SceneReader.h>
#include <RayTracer/RenderTarget/RayTraceImage.h>
#include <RayTracer/Scene/Camera.h>
#include <RayTracer/Scene/Primitives/Primitive.h>
#include <RayTracer/Scene/Material.h>

Scene::Scene() {
    // default values
    _hasSize  = false;
    _maxdepth = 5;
    _camera = nullptr;
}
Scene::~Scene() {
    for (auto prim : _primitives)
        delete prim;

    for (auto mat : _materials)
        delete mat;
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
    float t_hit = FLT_MAX;
    Intersection intersection;

    if (depth <= this->_maxdepth) {
        float t;
        Intersection hit;
        for (std::vector<Primitive*>::iterator it = _primitives.begin(); it < _primitives.end(); ++it) {
            t = (*it)->Intersect(ray, hit, t_hit);
            if (t > 0 && t < t_hit) {
                t_hit = t;
                intersection = hit;
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

    for (std::vector<Light>::iterator it = _lights.begin(); it != _lights.end(); ++it) {
        vec3 dir_to_light = it->LightVectorFrom(Hit.hitPoint);

        // light in front of object?
        if (dot(Hit.normal, dir_to_light) > 0) {
            Ray r(Hit.hitPoint + 0.001f*dir_to_light, dir_to_light);
        
            // distance from hitpoint to light
            float max_dist;
            if (it->pos_or_dir[3] == 1) {
                // point light
                max_dist = glm::distance(vec3(it->pos_or_dir), Hit.hitPoint);
            }
            else {
                // directional light, distance = infinite
                max_dist = FLT_MAX;
            }

            // Hitpoint ist im Schatten wenn inShadow die erste Intersection zwischen Hitpoint und Light position findet
            auto is_in_shadow = inShadow(r, max_dist);
            if (is_in_shadow)
                continue;       // pixel in shadow don't contribute to output color

            vec3 L = it->intensity;

            // only consider point light for attenuation (directional light would be 1 0 0 -> no change)
            if (it->pos_or_dir[3] == 1) {
                float d = glm::distance(Hit.hitPoint, vec3(it->pos_or_dir));
                L = it->intensity / (it->attenuation[0] + it->attenuation[1] * d + it->attenuation[2] * d * d);
            }

            // diffuse term
            float dotP = dot(Hit.normal, dir_to_light);
            out_color += L * (diffuse * common::max(dotP, 0.0f));

            // specular term
            vec3 halfVec = normalize(dir_to_light + -ray.dir);
            float halfAngle = dot(Hit.normal, halfVec);
            out_color += L * (specular * pow(common::max(halfAngle, 0.0f), shininess));
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
