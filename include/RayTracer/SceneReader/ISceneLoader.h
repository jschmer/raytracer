/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#pragma once
#include <memory>

class Scene;

/*
 * @brief   Interface for a scene reader
 */
class ISceneLoader
{
public:
    virtual std::unique_ptr<Scene> load(std::unique_ptr<Scene> scene) const = 0;
};

