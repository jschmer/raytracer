#pragma once
#include <string>
#include <sstream>
#include <vector>

#include <RayTracer/SceneReader/ISceneLoader.h>

class Scene;

/*
 * @brief   parses a Wavefront OBJ file
 */
class AssimpParser : public ISceneLoader
{
public:
    AssimpParser(std::string sceneFile)
        : sceneFile(sceneFile)
    {
    }

    ~AssimpParser() {}

    std::unique_ptr<Scene> load(std::unique_ptr<Scene> scene) const override;

private:
    std::string sceneFile;
};

