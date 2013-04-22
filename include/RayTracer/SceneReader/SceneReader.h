/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once

#include <string>
#include <memory>

class Scene;

std::unique_ptr<Scene> loadScene(const std::string scenefile);