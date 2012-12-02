#pragma once
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
        float hitPoint;

        while(_image->getSample(sample)) {          // OK
            _camera->generateRay(sample, ray);      // OK

            Primitive* hitObj = trace(ray, hitPoint);

            vec3 color(0.0);
            if (hitObj)
                color = shade(hitObj, hitPoint, ray);

            _image->commit(sample, color);  // OK
        }

        _image->save(_outputFilename);
    }

    vec3 shade(Primitive* obj, float t, Ray r) {
        vec3 color(0.0);
        color += obj->ambient;
        color += obj->emission;

        vec3 hitPoint = r.pos + r.dir * t;

        for (std::vector<Light>::iterator it = _lights.begin(); it != _lights.end(); ++it) {
            vec3 dir_to_light = normalize(it->LightVectorFrom(hitPoint));

            Ray r(hitPoint + 0.1f*dir_to_light, dir_to_light);

            float tmp;
            Primitive* hit = trace(r, tmp);

            if (hit)
                // in shadow, continue
                continue;

            float normal_dot_lightray = dot(dir_to_light, obj->Normal(hitPoint));
            vec3 diffuse = obj->diffuse * common::max(normal_dot_lightray, 0.0f);

            // half vec = eyepos - currentpos
            vec3 half_angle(0.0);
            vec3 specular(0.0); // = obj->specular * pow(common::max((float) dot(half_angle, obj->Normal()), 0.0f) ,obj->shininess);

            color += it->color * (diffuse + specular);
        }

        return color;
    }

    Primitive* trace(Ray &ray, float &t_hit) {
        t_hit = FLT_MAX;
        Primitive *hitObject = nullptr;

        float t;
        for (std::vector<Primitive*>::iterator it = _primitives.begin(); it < _primitives.end(); ++it) {
            t = (*it)->Intersect(ray);
            if (t > 0 && t < t_hit) {
                t_hit = t;
                hitObject = *it;
            }
        }

        return hitObject;
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

