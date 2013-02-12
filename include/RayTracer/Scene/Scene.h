#pragma once
#define GLM_SWIZZLE_XYZW 
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include <RayTracer/Ray.h>
#include <RayTracer/Scene/Geometry.h>
#include <RayTracer/Scene/Camera.h>

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

    ////////// scene variables
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

