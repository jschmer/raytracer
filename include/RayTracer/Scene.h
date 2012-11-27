#pragma once
#include <FreeImage\FreeImage.h>
#include <glm\glm.hpp>

#include <string>
#include <vector>

#include "SceneParser.h"
#include "RayTraceImage.h"

using namespace glm;

class Scene
{
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

    void saveImage() {
        FreeImage_Initialise();

        // save the rendered image
        {
            image->fill();

            FIBITMAP *img = FreeImage_ConvertFromRawBits(image->getByteBuffer(), _size.width, _size.height, _size.width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
            FreeImage_Save(FIF_PNG, img, _outputFilename.c_str(), 0);
        }

        FreeImage_DeInitialise();
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

