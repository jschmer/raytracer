#pragma once
#define GLM_SWIZZLE_XYZW 
#include <glm\glm.hpp>

#include <string>
#include <vector>
#include <iostream>

#include "SceneParser.h"
#include "RayTraceImage.h"
#include "Geometry.h"
#include "Camera.h"
#include "Ray.h"

using namespace glm;

class Scene
{
    friend class SceneParser;

    struct Intersection {
        Primitive *obj;
        vec3 hitPoint;
        float t;
    };

public:
    Scene() {
        // default values
        _maxdepth = 5;
        _outputFilename = "SceneRender.png";

        _image = NULL;
        _camera = NULL;

        _size.width  = 200;
        _size.height = 200;
    }
    ~Scene() {
        if (_camera)
            delete _camera;
    }

    void loadScene(std::string sceneFile) {
        SceneParser parser(sceneFile);
        *this = *parser.load();

        if (_image)
            delete _image;
        _image = new RayTraceImage(_size.width, _size.height);
    }

    void render() {
        _camera->initFov((float)_size.width, (float)_size.height);
        Sample sample;
        Ray ray;
        vec3 hitPoint;

        while(_image->getSample(sample)) {          // OK
            _camera->generateRay(sample, ray);      // OK

            Intersection Hit = trace(ray);

            vec3 color(0.0);
            if (Hit.obj)
                color = shade(Hit, ray, _camera->eye);

            _image->commit(sample, color);  // OK
        }

        _image->save(_outputFilename);
    }

    vec3 shade(Intersection &Hit, Ray r, vec3 eye) {
        vec3 color(0.0);
        color += Hit.obj->ambient;
        color += Hit.obj->emission;

        //Hit.hitPoint = r.pos + Hit.t * r.dir;
        //vec3 hitPoint = r.pos + r.dir * t;

        for (std::vector<Light>::iterator it = _lights.begin(); it != _lights.end(); ++it) {
            vec3 dir_to_light = it->LightVectorFrom(Hit.hitPoint);

             Ray r(Hit.hitPoint + 0.1f*dir_to_light, dir_to_light);

            Intersection hit = trace(r);

            if (hit.obj)
                // in shadow, continue
                continue;

            // diffuse term
            float normal_dot_lightray = dot(Hit.obj->Normal(Hit.hitPoint), dir_to_light);
            vec3 diffuse = Hit.obj->diffuse * common::max(normal_dot_lightray, 0.0f);

            // specular term
            // half vec = eyepos - currentpos
            //vec3 eyevec = normalize(eye - hitPoint);
            //vec3 halfVec = normalize(dir_to_light + eyevec);
            //float halfAngle = dot(obj->Normal(hitPoint), halfVec);
            vec3 specular(0); // = obj->specular * pow(common::max(halfAngle, 0.0f), obj->shininess);

            color += it->color * (diffuse + specular);
        }

        return color;
    }

    Intersection trace(Ray &ray) {
        float t_hit = FLT_MAX;
        Primitive *hitObject = nullptr;
        vec3 retHitPoint(0);

        float t;
        vec3 hitPoint(0);
        for (std::vector<Primitive*>::iterator it = _primitives.begin(); it < _primitives.end(); ++it) {
            t = (*it)->Intersect(ray, hitPoint);
            if (t > 0 && t < t_hit) {
                t_hit = t;
                hitObject = *it;
                retHitPoint = hitPoint;
            }
        }

        Intersection ret;
        ret.obj = hitObject;
        ret.t   = t_hit;
        ret.hitPoint = retHitPoint;
        return ret;
    }


    ////////// scene variables

    /*
     * raytraced output
     */
    RayTraceImage *_image;

    /*
     * General
     */
    struct {
        int width;
        int height;
    } _size;

    int _maxdepth;
    std::string _outputFilename;

    /*
     * Camera
     */
    Camera *_camera;

    /*
     * Geometry
     */
    std::vector<Primitive*> _primitives;

    /*
     * Lights
     */
    std::vector<Light> _lights;
};

