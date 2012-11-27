#pragma once
#include <string>
#include <vector>
#include <glm\glm.hpp>

#include "SceneParser.h"

using namespace glm;

class Scene
{
public:
    Scene() {
        // default values
        _maxdepth = 5;
        _outputFilename = "SceneRender.png";
    }
    ~Scene() {}

    void loadScene(std::string sceneFile) {
        SceneParser parser(sceneFile);
        *this = *parser.load();
    }

    /*
     * ray tracing loop
     */
    void render() {
        /*
        RayTraceImage rayimg(width, height);
        Sample sample;
        Ray ray;
        Color color;

        while (sample = rayimg.getSample()) {
            ray = camera.generateRay(sample);
            color = raytracer.trace(ray);
            rayimg.commit(sample, color);
        }

        film.writeImage();
        */
    }

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
    struct {
        int lookfromx;
        int lookfromy;
        int lookfromz;

        int lookatx;
        int lookaty;
        int lookatz;

        int upx;
        int upy;
        int upz;

        float fov;
    } _camera;

    /*
     * Geometry
     */
    struct Sphere {
        int x, y, z;
        float radius;
    };

    class Triangle {
    public:
        Triangle(vec3 &f, vec3 &g, vec3 &h) {
            x = f;
            y = g;
            z = h;
            faceNormal = glm::normalize(glm::cross(z - x, y - x)); 
        }
        vec3 x, y, z;
        vec3 faceNormal;
    };

    std::vector<Sphere>   _spheres;
    std::vector<vec3>     _vertices;
    std::vector<Triangle> _triangles;

    /*
     * Lights
     */

    /*
     * Materials
     */
    
};

