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
    : scene_()
{}

RayTracer::~RayTracer()
{}

void RayTracer::load(std::string scene_file) {
    try {
        scene_ = loadScene(scene_file);
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        scene_ = nullptr;
    }
}

void RayTracer::renderInto(IRenderTarget* render_target) {
    if (nullptr == scene_)
        return;

    auto& camera = *scene_->_camera;
    IRenderTarget& target = *render_target;
    
    // if the scene definition has a size
    // then initialize the target accordingly
    if (scene_->hasSize()) {
        // set size on target
        auto& size = scene_->_size;
        target.init(size.width, size.height);
    }

    camera.initFov(static_cast<float>(target.width()), static_cast<float>(target.height()));

    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    auto numCPU = sysinfo.dwNumberOfProcessors;

    // decrease process priority to prevent system hangs
    /*
    REALTIME_PRIORITY_CLASS------highest
    HIGH_PRIORITY_CLASS
    ABOVE_NORMAL_PRIORITY_CLASS
    NORMAL_PRIORITY_CLASS
    BELOW_NORMAL_PRIORITY_CLASS
    IDLE_PRIORITY_CLASS------lowest
    */
    SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);

    // load all samples
    std::vector<Sample> samples;
    typedef std::vector<Sample>::iterator SampleIterator;

    Sample sample;
    while(target.getSample(sample))
        samples.push_back(sample);

    auto thread_func = [&](SampleIterator begin, SampleIterator end){
        Ray ray;
        while (begin != end) {
            camera.generateRay(*begin, ray);

            Intersection Hit = scene_->trace(ray, 0);

            target.commit(*begin, Hit.color);

            // next sample!
            ++begin;
        }
    };

    auto dataset_size = samples.size() / numCPU;
    unsigned int begin = 0;
    unsigned int end = dataset_size;

    // start rendering
    auto start = std::chrono::system_clock::now();
    std::vector<std::future<void>> futs;
    while (numCPU > 0) {
        // start thread
        futs.push_back(std::async(thread_func, samples.begin() + begin, samples.begin() + end));

        begin += dataset_size;
        end   += dataset_size;
        --numCPU;
    }

    for (auto& fut : futs)
        fut.get();

    target.done();

    // rendering done!
    auto diff = std::chrono::system_clock::now() - start;
    render_duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
}

std::chrono::milliseconds RayTracer::getRenderDuration() const {
    return render_duration_;
}

void RayTracer::stop() {

}
