#include <RayTracer\Scene.h>

#include <Windows.h>
#include <iostream>
#include <future>

#include <RayTracer/SceneReader/SceneReader.h>
#include <RayTracer\SceneReader\SceneParser.h>
#include <RayTracer\Output\RayTraceImage.h>
#include <RayTracer\Camera.h>

Scene::Scene() {
    // default values
    _maxdepth = 5;

    _image = nullptr;
    _camera = nullptr;

    _size.width  = 600;
    _size.height = 600;
}
Scene::~Scene() {
    delete _camera;
}

void Scene::load(std::string sceneFile) {
    auto scene = loadScene(sceneFile);

    // copy scene content
    *this = *scene;

    // release scene from automatic memory management
    scene.release();

    // set output filename to inpuptfilename with png extension
    auto point_pos = sceneFile.find_last_of('.');
    if (point_pos == std::string::npos)
        throw std::exception("Couldn't find a file extension?!");

    _outputFilename = sceneFile.substr(0, point_pos) + ".png";

    delete _image;
    _image = new RayTraceImage(_size.width, _size.height, _outputFilename);
}

void Scene::render() {
    _camera->initFov((float)_size.width, (float)_size.height);

    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    auto numCPU = sysinfo.dwNumberOfProcessors;

    // decrease process priority to prevent hangs of the system
    SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
    /*
    REALTIME_PRIORITY_CLASS------highest
    HIGH_PRIORITY_CLASS
    ABOVE_NORMAL_PRIORITY_CLASS
    NORMAL_PRIORITY_CLASS
    BELOW_NORMAL_PRIORITY_CLASS
    IDLE_PRIORITY_CLASS------lowest
    */

    // load all samples
    std::vector<Sample> samples;
    typedef std::vector<Sample>::iterator SampleIterator;

    Sample sample;
    while(_image->getSample(sample))
        samples.push_back(sample);

    auto thread_func = [=](SampleIterator begin, SampleIterator end){
        Ray ray;
        while (begin != end) {
            this->_camera->generateRay(*begin, ray);

            Intersection Hit = this->trace(ray, 0);

            this->_image->commit(*begin, Hit.color);

            // next sample!
            ++begin;
        }
    };

    auto dataset_size = samples.size() / numCPU;
    unsigned int begin = 0;
    unsigned int end = dataset_size;

    std::vector<std::future<void>> futs;
    while (numCPU > 0) {
        // start thread
        futs.push_back(std::async(thread_func, samples.begin() + begin, samples.begin() + end));

        begin += dataset_size;
        end += dataset_size;
        --numCPU;
    }

    for (auto& fut : futs)
        fut.get();

    _image->save();
}

Intersection Scene::inShadow(Ray const &ray, float t_hit = FLT_MAX) {
    Intersection ret;
    ret.obj = nullptr;

    float t;
    for (std::vector<Primitive*>::iterator it = _primitives.begin(); it < _primitives.end(); ++it) {
        Intersection hit;
        t = (*it)->Intersect(ray, hit);
        if (t > 0 && t < t_hit) {
            t_hit = t;
            ret = hit;
        }
    }

    return ret;
}

Intersection Scene::trace(Ray const &ray, int depth) {
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

vec3 Scene::shade(Intersection &Hit, Ray const &ray, int depth) {
    vec3 color(0.0);
    color += Hit.obj->ambient;
    color += Hit.obj->emission;

    for (std::vector<Light>::iterator it = _lights.begin(); it != _lights.end(); ++it) {
        vec3 dir_to_light = it->LightVectorFrom(Hit.hitPoint);

        // light in front of object?
        if (dot(Hit.normal, dir_to_light) > 0) {
            Ray r(Hit.hitPoint +0.001f*dir_to_light, dir_to_light);
        
            Intersection ShadowHit = inShadow(r);
            if (ShadowHit.obj) {
                // if i'm testing against point light: check if intersection is between startpoint and light!
                // the startpoint is in shadow only then!
                if (it->pos_or_dir[3] == 1) {
                    float len_to_light        = glm::distance(vec3(it->pos_or_dir), Hit.hitPoint);
                    float len_to_intersection = glm::distance(ShadowHit.hitPoint, Hit.hitPoint);

                    if (len_to_intersection <= len_to_light)
                        // Hit.hitPoint in shadow, next light
                        continue;  
                } else {
                    // intersection with directional light
                    continue;
                }
            }

            vec3 L = it->color;

            // only consider point light for attenuation (directional light would be 1 0 0 -> no change)
            if (it->pos_or_dir[3] == 1) {
                float d = glm::distance(Hit.hitPoint, vec3(it->pos_or_dir));
                L = it->color / (it->attenuation[0] + it->attenuation[1] * d + it->attenuation[2] * d * d);
            }

            // diffuse term
            float dotP = dot(Hit.normal, dir_to_light);
            color += L * (Hit.obj->diffuse * common::max(dotP, 0.0f));

            // specular term
            vec3 halfVec = normalize(dir_to_light + -ray.dir);
            float halfAngle = dot(Hit.normal, halfVec);
            color += L * (Hit.obj->specular * pow(common::max(halfAngle, 0.0f), Hit.obj->shininess));
        }
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
