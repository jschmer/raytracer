#pragma once

#include <string>
#include <memory>
#include <chrono>

#include <RayTracer/RenderTarget/IRenderTarget.h>

class Scene;

class RayTracer {
public:
    RayTracer();
    ~RayTracer();

    void load(std::string scene_file);
    void renderInto(IRenderTarget* target);
    void stop();

    std::chrono::milliseconds getRenderDuration() const;

private:
    std::unique_ptr<Scene> scene_;
    std::chrono::milliseconds render_duration_;
};