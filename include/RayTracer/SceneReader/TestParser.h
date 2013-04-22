/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once
#include <string>
#include <sstream>
#include <vector>

#include <RayTracer/SceneReader/ISceneLoader.h>

class Scene;

/*
 * @brief   parses a ".test" scene file used in edx course "3D Graphics"
 */
class TestParser : public ISceneLoader
{
public:
    TestParser(std::string sceneFile)
        : sceneFile(sceneFile)
    {
    }

    ~TestParser() {}

    std::unique_ptr<Scene> load(std::unique_ptr<Scene> scene) const override;

private:
    std::string sceneFile;

    bool readvals(std::stringstream &s, const int numvals, std::vector<float> &values) const;
};
