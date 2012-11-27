#pragma once
#include <glm\glm.hpp>

#include <string>
#include <vector>

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

        image = NULL;

        _size.width  = 200;
        _size.height = 200;
    }
    ~Scene() {}



    void loadScene(std::string sceneFile) {
        SceneParser parser(sceneFile);
        *this = *parser.load();

        if (image)
            delete image;
        image = new RayTraceImage(_size.width, _size.height);
    }

    /*
     * ray tracing loop
     */
    void render() {
        Sample sample;
        Ray ray;
        vec3 color;

        while (image->getSample(sample)) {
            ray = _camera.generateRay(sample);
            color = trace(ray);
            //rayimg.commit(sample, color);
        }

        image->save(_outputFilename);
    }

    vec3 trace(Ray &ray) {

    }



    ////////// scene variables

    /*
     * raytraced output
     */
    RayTraceImage *image;

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
    Camera _camera;

    /*
     * Geometry
     */
    std::vector<Sphere>   _spheres;
    std::vector<Triangle> _triangles;

    /*
     * Lights
     */
    std::vector<Light> _lights;
};

