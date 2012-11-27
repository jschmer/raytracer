#include "..\include\RayTracer\SceneParser.h"

#include <RayTracer\SceneParser.h>
#include <RayTracer\Scene.h>

#include "StringSplit.h"

Scene* SceneParser::load(){
    Scene *s = new Scene();

    std::vector<std::string> path = StringSplit::split(sceneFile, '/');

    s->_outputFilename = path.back() + ".png";
    
    return s;
}