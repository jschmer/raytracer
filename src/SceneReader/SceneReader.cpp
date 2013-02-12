#include <RayTracer/SceneReader/SceneReader.h>

#include <algorithm>

#include <RayTracer/Scene/Scene.h>
#include <RayTracer/SceneReader/ObjParser.h>
#include <RayTracer/SceneReader/SceneParser.h>

std::unique_ptr<Scene> loadScene(const std::string scenefile) {
    auto point_pos = scenefile.find_last_of('.');

    if (point_pos == std::string::npos) {
        throw std::exception("I can't load this file type!");
    }
    std::string ext = scenefile.substr(point_pos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext.compare(".obj") == 0) {
        return ObjParser(scenefile).load();
    }
    else if (ext.compare(".test") == 0) {
        return SceneParser(scenefile).load();
    }
    else {
        throw std::exception("I can't load this file type!");
    }
}