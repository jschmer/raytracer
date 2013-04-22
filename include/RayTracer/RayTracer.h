/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
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
    float getFPS();
    std::chrono::milliseconds getRenderDuration() const;

private:
    std::unique_ptr<Scene>    _scene;
    std::chrono::milliseconds _render_duration;
    IRenderTarget*            _target;
    unsigned long             _numCPUs;
    float                     _fps;
};