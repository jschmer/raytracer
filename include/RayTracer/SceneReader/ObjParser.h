#pragma once
#include <string>
#include <sstream>
#include <vector>

#include <RayTracer/SceneReader/ISceneReader.h>

class Scene;

/*
 * @brief   parses a 3ds Max Wavefront OBJ file
 */
class ObjParser : public ISceneReader
{
public:
    ObjParser(std::string sceneFile)
        : sceneFile(sceneFile)
    {
    }

    ~ObjParser() {}

    Scene* load() override;

private:
    std::string sceneFile;

    bool readvals(std::stringstream &s, const int numvals, std::vector<float> &values);
};

