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

        while(_image->getSample(sample)) {          // OK
            _camera->generateRay(sample, ray);      // OK

            Primitive* hitObj = trace(ray);

            vec3 color(0.0);
            if (hitObj)
                color = hitObj->MaterialColor();

            _image->commit(sample, color);  // OK
        }

        _image->save(_outputFilename);
    }

    Primitive* trace(Ray &ray) {
        float mindist = FLT_MAX;
        Primitive *hitObject = nullptr;

        float t;
        for (std::vector<Primitive*>::iterator it = _primitives.begin(); it < _primitives.end(); ++it) {
            t = (*it)->Intersect(ray);
            if (t > 0 && t < mindist) {
                mindist = t;
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

