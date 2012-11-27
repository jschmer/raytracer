#pragma once
#include <string>
#include "Scene.h"

/*
 * @brief   parses given config file and populates a new config class
 */
class SceneParser
{
public:
    SceneParser(std::string configFile)
        : configFile(configFile)
    {}

    ~SceneParser() {}

    Scene parse();

private:
    std::string configFile;
};

