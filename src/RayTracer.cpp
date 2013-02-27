#include <RayTracer/RayTracer.h>

// system header
#include <RayTracer/windows_includes.h>
#include <iostream>
#include <future>
#include <vector>

// other framework header
#include <concurrent_queue.h>

// project header
#include <RayTracer/Scene/Scene.h>
#include <RayTracer/Scene/Camera.h>
#include <RayTracer/Scene/Primitives/Primitive.h>
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

    // start timing
    auto start = std::chrono::system_clock::now();

    // Compute Axis aligned bounding boxes
    // for each Primitive and 
    // construct some kind of hirarchy tree from them
    // for example a 'fixed' grid (divide scene bounding box into 10*10*10 boxes)
    scene_->createAABB();

    IRenderTarget& target = *render_target;
    
    // if the scene definition has a size
    // then initialize the target accordingly
    if (scene_->hasSize()) {
        // set size on target
        auto& size = scene_->_size;
        target.init(size.width, size.height);
    }

    if (!scene_->_camera)
        scene_->createDefaultCamera();

    auto& camera = *scene_->_camera;
    camera.initFov(static_cast<float>(target.width()), static_cast<float>(target.height()));

    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    auto numCPU = sysinfo.dwNumberOfProcessors;

#ifdef MULTICORE_DISABLED
    numCPU = 1;
#endif

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
    // TODO: split into tiles (32x32) and package them that each thread can work on one package
    //       use a concurrent_queue
    typedef std::vector<Sample> SamplePack;
    concurrency::concurrent_queue<SamplePack> sample_packs;
    
    const auto Packsize = 32u;

    // generating sample packs
    SamplePack pack;
    Sample sample;
    bool clear = false;
    while(target.getSample(sample)) {
        pack.push_back(sample);
        clear = false;
        if (pack.size() == Packsize*Packsize) {
            sample_packs.push(SamplePack(pack));
            pack.clear();
            clear = true;
        }
    }
    if (!clear)
        sample_packs.push(SamplePack(pack));

    auto thread_func = [&](){
        Ray ray;
        SamplePack pack;
        while (sample_packs.try_pop(pack)) {
            for (auto& sample : pack) {
                camera.generateRay(sample, ray);

                Intersection Hit = scene_->trace(ray, 0);

                target.commit(sample, Hit.color);
            }
        }
    };

    std::vector<std::future<void>> futs;
    while (numCPU > 0) {
        // start thread
        futs.push_back(std::async(thread_func));
        --numCPU;
    }

    for (auto& fut : futs)
        fut.get();

    target.OnDone();

    // rendering done!
    auto diff = std::chrono::system_clock::now() - start;
    render_duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
}

std::chrono::milliseconds RayTracer::getRenderDuration() const {
    return render_duration_;
}

void RayTracer::stop() {

}
