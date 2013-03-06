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
    : _scene(),
    _target(nullptr),
    _fps(0.0f)
{
#ifdef MULTICORE_DISABLED
    _numCPUs = 1;
#else
    // get number of cores/processors
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    _numCPUs = sysinfo.dwNumberOfProcessors;
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
}

RayTracer::~RayTracer()
{}

void RayTracer::load(std::string scene_file) {
    try {
        _scene = loadScene(scene_file);

        // Compute Axis aligned bounding boxes
        // for each Primitive and 
        // construct some kind of hirarchy tree from them
        // for example a 'fixed' grid (divide scene bounding box into 10*10*10 boxes)
        _scene->createAABB();

        _target = nullptr;

    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        _scene = nullptr;
        throw e;
    }
}

void RayTracer::renderInto(IRenderTarget* render_target) {
    if (nullptr == _scene)
        return;

    // aliases
    IRenderTarget& target = *render_target;

    // new render target? Initialize target and camera with scene parameters 
    if (_target != render_target) {
        // if the scene definition has a size
        // then initialize the target accordingly
        auto& size = _scene->_size;
        if (_scene->hasSize()) {
            // set size on target
            target.init(size.width, size.height);
        } else {
            size.height = target.height();
            size.width  = target.width();
            _scene->_hasSize = true;
        }

        if (!_scene->_camera)
            _scene->createDefaultCamera();

        _scene->_camera->initFov(static_cast<float>(size.width), static_cast<float>(size.height));

        _target = render_target;
    }

    // start timing
    auto start = std::chrono::system_clock::now();

    // load all samples
    // TODO: split into tiles (32x32) and package them that each thread can work on one package
    //       use a concurrent_queue
    typedef std::vector<Sample> SamplePack;
    concurrency::concurrent_queue<SamplePack> sample_packs;
    const auto Packsize = 1024u;

    // generating sample packs
    SamplePack pack;
    Sample sample;
    while(target.getSample(sample)) {
        pack.push_back(sample);
        if (pack.size() == Packsize) {
            sample_packs.push(SamplePack(pack));
            pack.clear();
        }
    }
    if (!pack.empty())
        sample_packs.push(SamplePack(pack));

    // aliases
    auto& camera = *_scene->_camera;

    // thread function == raytrace loop
    auto thread_func = [&](){
        Ray ray;
        SamplePack pack;
        while (sample_packs.try_pop(pack)) {
            for (auto& sample : pack) {
                camera.generateRay(sample, ray);

                Intersection Hit = _scene->trace(ray, 0);

                target.commit(sample, Hit.color);
            }
        }
    };

    std::vector<std::future<void>> futs;
    auto numCPUs = _numCPUs;
    while (numCPUs > 0) {
        // start thread for every processor/core
        futs.push_back(std::async(thread_func));
        --numCPUs;
    }

    // wait for all threads to finish
    for (auto& fut : futs)
        fut.get();

    // call done method on target
    target.OnDone();

    // rendering done! compute render time
    auto diff = std::chrono::system_clock::now() - start;
    _render_duration = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    // and fps
    auto rendertime_ms = _render_duration.count();
    auto this_fps = 1000.0f / rendertime_ms;
    _fps = (_fps + this_fps)/2.0f;
}

float RayTracer::getFPS() const {
    return _fps;
}

std::chrono::milliseconds RayTracer::getRenderDuration() const {
    return _render_duration;
}

void RayTracer::stop() {

}

void RayTracer::moveCamera(Direction dir) const {
    auto& scene = *_scene;

    if (scene._camera != nullptr) {
        const float move_amount = 1.0f;
        scene.moveCamera(dir, move_amount);
    }
}

void RayTracer::moveCrystalBall(Direction dir) const {
    auto& scene = *_scene;

    if (scene._camera != nullptr) {
        const float move_amount = 1.0f;
        scene._camera->moveAroundCrystalBall(dir, move_amount);
    }
}