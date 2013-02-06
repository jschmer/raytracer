#pragma once
class Scene;

/*
 * @brief   Interface for a scene reader
 */
class ISceneReader
{
public:
    virtual Scene* load() = 0;
};

