#pragma once

#include <string>
#include <memory>
#include <chrono>

#include <RayTracer/RenderTarget/IRenderTarget.h>
#include <RayTracer/Scene/Scene.h>

class RayTracer {
public:
    RayTracer();
    ~RayTracer();

    void load(std::string scene_file);
    void renderInto(IRenderTarget* target);

    void stop();
    void start();

    // Getter
    float getFPS() const;
    std::chrono::milliseconds getRenderDuration() const;

    // Camera transformation
    void moveCamera(Direction dir) const;
    void moveCrystalBall(Direction dir) const;

private:
    void initRendertarget(IRenderTarget* target);

private:
    std::unique_ptr<Scene>    _scene;
    std::chrono::milliseconds _render_duration;
    IRenderTarget*            _target;
    unsigned long             _numCPUs;
    float                     _fps;
    bool                      _stop;
};