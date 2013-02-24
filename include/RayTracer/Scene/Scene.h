#pragma once

// system header
#include <string>
#include <vector>
#include <memory>

#include <RayTracer/glm_includes.h>
#include <RayTracer/Scene/Primitives/AABB.h>

// forward declarations
class Ray;
class Light;
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

    void createAABB();

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
    AABB _scene_aabb;
    std::vector<AABB> _aabbs;

    /*
     * Materials
     */
    std::vector<Material*> _materials;

    /*
     * Lights
     */
    std::vector<Light*> _lights;
};

