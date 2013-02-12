#pragma once

#include <string>
#include <memory>

#include <RayTracer/RenderTarget/IRenderTarget.h>

class Scene;

class RayTracer {
public:
    RayTracer();
    ~RayTracer();

    void load(std::string scene_file);
    void renderInto(IRenderTarget* target);
    void stop();

private:
    std::unique_ptr<Scene> scene;
};