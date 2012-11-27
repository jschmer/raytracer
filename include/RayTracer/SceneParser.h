#pragma once
#include <string>
#include <sstream>
#include <vector>

using std::vector;
using std::stringstream;

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

    bool readvals (stringstream &s, const int numvals, vector<float> &values);
};

