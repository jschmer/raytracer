#include <RayTracer\Scene.h>

#include <RayTracer\SceneParser.h>
#include <RayTracer\RayTraceImage.h>
#include <RayTracer\Camera.h>

#include <iostream>

Scene::Scene() {
    // default values
    _maxdepth = 5;
    _outputFilename = "SceneRender.png";

    _image = NULL;
    _camera = NULL;

    _size.width  = 200;
    _size.height = 200;
}
Scene::~Scene() {
    if (_camera)
        delete _camera;
}

void Scene::loadScene(std::string sceneFile) {
    SceneParser parser(sceneFile);
    *this = *parser.load();

    if (_image)
        delete _image;
    _image = new RayTraceImage(_size.width, _size.height);
}

void Scene::render() {
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


Intersection Scene::trace(Ray &ray) {
    float t_hit = FLT_MAX;
    Intersection ret;
    ret.obj = nullptr;

    float t;
    Intersection hit;
    for (std::vector<Primitive*>::iterator it = _primitives.begin(); it < _primitives.end(); ++it) {
        t = (*it)->Intersect(ray, hit);
        if (t > 0 && t < t_hit) {
            t_hit = t;
            ret = Intersection(hit);
        }
    }

    return ret;
}

vec3 Scene::shade(Intersection &Hit, Ray r, vec3 eye) {
    vec3 color(0.0);
    color += Hit.obj->ambient;
    color += Hit.obj->emission;

    for (std::vector<Light>::iterator it = _lights.begin(); it != _lights.end(); ++it) {
        vec3 dir_to_light = it->LightVectorFrom(Hit.hitPoint);

        Ray r(Hit.hitPoint + 0.1f*dir_to_light, dir_to_light);

        Intersection hit = trace(r);

        if (hit.obj)
            // in shadow, continue
            continue;

        // diffuse term
        float normal_dot_lightray = dot(Hit.normal, dir_to_light);
        vec3 diffuse = Hit.obj->diffuse * common::max(normal_dot_lightray, 0.0f);

        // specular term
        // half vec = eyepos - currentpos
        vec3 eyevec = normalize(eye - Hit.hitPoint);
        vec3 halfVec = normalize(dir_to_light + eyevec);
        float halfAngle = dot(Hit.normal, halfVec);
        vec3 specular = Hit.obj->specular * pow(common::max(halfAngle, 0.0f), Hit.obj->shininess);

        color += it->color * (diffuse + specular);
    }

    return color;
}
