/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

#include <string>
#include <vector>

#include <RayTracer/SceneReader/ISceneLoader.h>

class Scene;

/*
 * @brief   parses a ".raytrace" scene file that supports embedding exactly one other scene file
 *          (e.g. *.obj, *.test, ...)
 */
class RaytraceParser : public ISceneLoader
{
public:
    RaytraceParser(std::string sceneFile)
        : sceneFile(sceneFile)
    {
    }

    ~RaytraceParser() {}

    std::unique_ptr<Scene> load(std::unique_ptr<Scene> scene) const override;

private:
    std::string sceneFile;
};
