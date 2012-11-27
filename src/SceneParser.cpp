#include "..\include\RayTracer\SceneParser.h"

#include <RayTracer\SceneParser.h>
#include <RayTracer\Scene.h>

Scene* SceneParser::load(){
    Scene *s = new Scene();
    s->_outputFilename = sceneFile + ".png";

    return s;
}