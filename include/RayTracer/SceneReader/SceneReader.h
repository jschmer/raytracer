#pragma once

#include <string>
#include <memory>

class Scene;

std::unique_ptr<Scene> loadScene(const std::string scenefile);