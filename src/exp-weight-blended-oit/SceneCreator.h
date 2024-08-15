#pragma once

#include <vector>

class VBObjectExt;

class SceneCreator
{
public:
    SceneCreator() = default;
    ~SceneCreator() = default;

    bool CreateOpaqueObjects(std::vector<std::shared_ptr<VBObjectExt>>& out);
    bool CreateTranslucentObjects(std::vector<std::shared_ptr<VBObjectExt>>& out);
};