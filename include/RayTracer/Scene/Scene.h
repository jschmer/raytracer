#pragma once

// system header
#include <string>
#include <vector>
#include <memory>

#include <RayTracer/glm_includes.h>
#include <RayTracer/Scene/Light.h>

// forward declarations
class Ray;
class Camera;
class Material;
class Primitive;
class RayTraceImage;
struct Intersection;

class Scene
{
public:
    Scene();
    ~Scene();

    color3 shade(Intersection &Hit, Ray const &ray, int depth);
    Intersection trace(Ray const &ray, int depth);
    bool inShadow(Ray const &ray, float t_hit);

    bool hasSize() const {
        return _hasSize;
    }

    ////////// scene variables
    int _maxdepth;
    std::string _outputFilename;

    /*
     * image size
     */
    struct size {
        unsigned int width, height;
    } _size;
    bool _hasSize;

    /*
     * Camera
     */
    std::unique_ptr<Camera> _camera;

    /*
     * Geometry
    */
    std::vector<Primitive*> _primitives;

    /*
     * Materials
     */
    std::vector<Material*> _materials;

    /*
     * Lights
     */
    std::vector<Light*> _lights;
};

