#pragma once
#include <string>
#include <sstream>
#include <vector>

#include <RayTracer/SceneReader/ISceneLoader.h>

class Scene;

/*
 * @brief   parses a Wavefront OBJ file
 */
class ObjParser : public ISceneLoader
{
public:
    ObjParser(std::string sceneFile)
        : sceneFile(sceneFile)
    {
    }

    ~ObjParser() {}

    std::unique_ptr<Scene> load() const override;

private:
    std::string sceneFile;

    bool readvals(std::stringstream &s, const int numvals, std::vector<float> &values) const;
};

