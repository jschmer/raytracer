#include <RayTracer/RayTracer.h>

// system header
#include <Windows.h>
#include <iostream>
#include <future>
#include <vector>

// other framework header

// project header
#include <RayTracer/Scene/Scene.h>
#include <RayTracer/SceneReader/SceneReader.h>
#include <String/StringHelper.h>

// class definition
RayTracer::RayTracer()
    : scene()
{}

RayTracer::~RayTracer()
{}

void RayTracer::load(std::string scene_file) {
    scene = loadScene(scene_file);
}

void RayTracer::renderInto(IRenderTarget* target) {
    scene->_camera->initFov(static_cast<float>(target->width), static_cast<float>(target->height));

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
    while(target->getSample(sample))
        samples.push_back(sample);

    auto thread_func = [=](SampleIterator begin, SampleIterator end){
        Ray ray;
        while (begin != end) {
            scene->_camera->generateRay(*begin, ray);

            Intersection Hit = scene->trace(ray, 0);

            target->commit(*begin, Hit.color);

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

    target->done();
}

void RayTracer::stop() {

}
