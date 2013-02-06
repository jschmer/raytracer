#pragma once
#include <string>
#include <sstream>
#include <vector>

#include <RayTracer/SceneReader/ISceneReader.h>

class Scene;

/*
 * @brief   parses a ".test" scene file used in edx course "3D Graphics"
 */
class SceneParser : public ISceneReader
{
public:
    SceneParser(std::string sceneFile)
        : sceneFile(sceneFile)
    {
    }

    ~SceneParser() {}

    Scene* load() override;

private:
    std::string sceneFile;

    bool readvals (std::stringstream &s, const int numvals, std::vector<float> &values);
};

