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

    while(_image->getSample(sample)) {
        _camera->generateRay(sample, ray);

        Intersection Hit = trace(ray, 0);

        _image->commit(sample, Hit.color);
    }

    _image->save(_outputFilename);
}

bool Scene::inShadow(Ray &ray) {
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

    // got a hit point -> get color!
    if (ret.obj) {
        return true;
    }

    return false;
}

Intersection Scene::trace(Ray &ray, int depth) {
    float t_hit = FLT_MAX;
    Intersection ret;
    ret.obj = nullptr;

    if (depth <= this->_maxdepth) {
        float t;
        Intersection hit;
        for (std::vector<Primitive*>::iterator it = _primitives.begin(); it < _primitives.end(); ++it) {
            t = (*it)->Intersect(ray, hit);
            if (t > 0 && t < t_hit) {
                t_hit = t;
                ret = hit;
            }
        }
    }

    // got a hit point -> get color!
    if (ret.obj) {
        ret.color = shade(ret, ray, depth);
    }

    return ret;
}

vec3 Scene::shade(Intersection &Hit, Ray &ray, int depth) {
    vec3 color(0.0);
    color += Hit.obj->ambient;
    color += Hit.obj->emission;

    for (std::vector<Light>::iterator it = _lights.begin(); it != _lights.end(); ++it) {
        vec3 dir_to_light = it->LightVectorFrom(Hit.hitPoint);

        Ray r(Hit.hitPoint + 0.01f*dir_to_light, dir_to_light);
        
        if (inShadow(r))
            // in shadow, next light
            continue;

        // diffuse term
        color += it->color * (Hit.obj->diffuse * common::max(dot(Hit.normal, dir_to_light), 0.0f));

        // specular term
        vec3 halfVec = normalize(dir_to_light + -ray.dir);
        float halfAngle = dot(Hit.normal, halfVec);
        color += it->color * (Hit.obj->specular * pow(common::max(halfAngle, 0.0f), Hit.obj->shininess));
    }

    // reflection
    // R = V – 2 * (V·N) * N 
    Ray reflectionRay;
    reflectionRay.dir = ray.dir - 2 * dot(ray.dir, Hit.normal) * Hit.normal;
    reflectionRay.pos = Hit.hitPoint + 0.01f*reflectionRay.dir;

    Intersection t = this->trace(reflectionRay, depth+1);
    if (t.obj)
        color += Hit.obj->specular * t.color;

    return color;
}
