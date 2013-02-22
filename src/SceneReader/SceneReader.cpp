#include <RayTracer/SceneReader/SceneReader.h>

#include <algorithm>

#include <assimp/Importer.hpp>

#include <RayTracer/Scene/Scene.h>
#include <RayTracer/SceneReader/TestParser.h>
#include <RayTracer/SceneReader/RaytraceParser.h>
#include <RayTracer/SceneReader/AssimpParser.h>

std::unique_ptr<Scene> loadScene(const std::string scenefile) {
    // getting file extension
    auto point_pos = scenefile.find_last_of('.');
    if (point_pos == std::string::npos) {
        throw std::exception("I can't load this file type!");
    }
    std::string ext = scenefile.substr(point_pos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // construct scene
    std::unique_ptr<Scene> scene(new Scene());

    // handle test files right from the start, we have our own parser
    if (ext.compare(".test") == 0) {
        return TestParser(scenefile).load(std::move(scene));
    }
    // handle custom file type here
    else if (ext.compare(".raytrace") == 0) {
        return RaytraceParser(scenefile).load(std::move(scene));
    }
    // let Assimp handle everything else
    else {
        // getting supported file types by assimp
        std::string ext_list;
        {
            Assimp::Importer loader;
            loader.GetExtensionList(ext_list);
        }

        // checking if ext is in ext_list
        auto ext_supported = ext_list.find(ext);
        if (ext_supported != std::string::npos) {
            // file type supported! load it!
            return AssimpParser(scenefile).load(std::move(scene));
        }
        else {
            // file type not supported!
            std::string message = ext + " types aren't supported!";
            throw std::exception(message.c_str());
        }
    }
}