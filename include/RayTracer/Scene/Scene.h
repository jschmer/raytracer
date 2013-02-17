#pragma once



#define GLM_SWIZZLE_XYZW 
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include <RayTracer/Ray.h>
#include <RayTracer/Scene/Primitives/Primitive.h>
#include <RayTracer/Scene/Camera.h>
#include <RayTracer/Scene/Light.h>
#include <RayTracer/Scene/Material.h>

class Camera;
class Primitive;
class RayTraceImage;

using namespace glm;

class Scene
{
public:
    Scene();
    ~Scene();

    vec3 shade(Intersection &Hit, Ray const &ray, int depth);
    Intersection trace(Ray const &ray, int depth);
    Intersection inShadow(Ray const &ray, float t_hit);

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
    Camera *_camera;

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
    std::vector<Light> _lights;
};

