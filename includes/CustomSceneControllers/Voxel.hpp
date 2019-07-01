
#pragma once

#include "AppEnv.hpp"

class VoxelSceneController: public ISceneController {
    std::shared_ptr<GLS::Node> mainChunkNode;
    std::shared_ptr<GLS::VoxelChunk> mainChunk;
    std::array<std::weak_ptr<GLS::VoxelChunk>, 6> neighbourgsChunks;
    double removeBlockCD;
    int i;

    public:
    VoxelSceneController(std::shared_ptr<GLSWindow> window);
    virtual ~VoxelSceneController();

    virtual void makeScene();
    virtual void update();
};
