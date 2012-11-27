#pragma once
#include <string>

class Scene;

/*
 * @brief   parses given config file and populates a new config class
 */
class SceneParser
{
public:
    SceneParser(std::string sceneFile)
        : sceneFile(sceneFile)
    {
    }

    ~SceneParser() {}

    Scene* load();

private:
    std::string sceneFile;
};

