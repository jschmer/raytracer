#pragma once

#include <string>
#include <algorithm>

#include "SceneParser.h"
#include "ObjParser.h"

class Scene;

Scene* loadScene(std::string scenefile) {
    std::string ext = scenefile.substr(scenefile.length()-4);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext.compare(".obj") == 0) {
        return ObjParser(scenefile).load();
    }
    else if (ext.compare(".scene") == 0) {
        return SceneParser(scenefile).load();
    }
    else {
        throw std::exception("I can't load this file type!");
    }
}