#include <RayTracer/Scene/Scene.h>

#include <iostream>
#include <future>

#include <RayTracer/SceneReader/SceneReader.h>
#include <RayTracer/SceneReader/SceneParser.h>
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
    delete _camera;

    for (auto prim : _primitives)
        delete prim;

    for (auto mat : _materials)
        delete mat;
}

Intersection Scene::inShadow(Ray const &ray, float t_hit = FLT_MAX) {
    Intersection ret;

    float t;
    for (std::vector<Primitive*>::iterator it = _primitives.begin(); it < _primitives.end(); ++it) {
        Intersection hit;
        t = (*it)->Intersect(ray, hit);
        if (t > 0 && t < t_hit) {
            t_hit = t;
            ret = hit;
        }
    }

    return ret;
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
        
            Intersection ShadowHit = inShadow(r);
            if (ShadowHit.has_hit) {
                // if i'm testing against point light: check if intersection is between startpoint and light!
                // the startpoint is in shadow only then!
                if (it->pos_or_dir[3] == 1) {
                    float len_to_light        = glm::distance(vec3(it->pos_or_dir), Hit.hitPoint);
                    float len_to_intersection = glm::distance(ShadowHit.hitPoint, Hit.hitPoint);

                    if (len_to_intersection <= len_to_light)
                        // Hit.hitPoint in shadow, next light
                        continue;  
                } else {
                    // intersection with directional light
                    continue;
                }
            }

            vec3 L = it->color;

            // only consider point light for attenuation (directional light would be 1 0 0 -> no change)
            if (it->pos_or_dir[3] == 1) {
                float d = glm::distance(Hit.hitPoint, vec3(it->pos_or_dir));
                L = it->color / (it->attenuation[0] + it->attenuation[1] * d + it->attenuation[2] * d * d);
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

    // reflection
    // R = V – 2 * (V·N) * N 
    Ray reflectionRay;
    reflectionRay.dir = ray.dir - 2 * dot(ray.dir, Hit.normal) * Hit.normal;
    reflectionRay.pos = Hit.hitPoint + 0.01f*reflectionRay.dir;

    
    Intersection t = this->trace(reflectionRay, depth+1);
    if (t.has_hit)
        out_color += specular * t.color;

    return out_color;
}
