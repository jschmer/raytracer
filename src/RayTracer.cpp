#include <RayTracer/RayTracer.h>

// compiler header
#include <iostream>

// other framework header

// project header
#include <RayTracer/Scene/Scene.h>
#include <RayTracer/SceneReader/SceneReader.h>

// temporary
#include <RayTracer/RenderTarget/RayTraceImage.h>

// class definition
RayTracer::RayTracer()
    : scene()
{}

RayTracer::~RayTracer()
{}

void RayTracer::load(std::string scene_file) {
    scene = loadScene(scene_file);

    // set output filename to inpuptfilename with png extension
    auto point_pos = scene_file.find_last_of('.');
    if (point_pos == std::string::npos)
        throw std::exception("Couldn't find a file extension?!");

    scene->_outputFilename = scene_file.substr(0, point_pos) + ".png";

    delete scene->_image;
    scene->_image = new RayTraceImage(scene->_outputFilename, scene->_size.width, scene->_size.height);
}

void RayTracer::renderInto(IRenderTarget* target) {

}

void RayTracer::stop() {

}
