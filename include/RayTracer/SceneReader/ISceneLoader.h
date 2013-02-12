#pragma once
#include <memory>

class Scene;

/*
 * @brief   Interface for a scene reader
 */
class ISceneLoader
{
public:
    virtual std::unique_ptr<Scene> load() const = 0;
};
