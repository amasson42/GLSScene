
#pragma once

#include "AppEnv.hpp"

void initNoise(unsigned int seed);
double smoothNoise(double x, double y, double z);
double linearNoise(double x, double y, double z);

class VoxelWorld;
class VoxelProceduralSceneController: public ISceneController {
    std::shared_ptr<GLS::Node> worldNode;
    std::shared_ptr<GLS::Node> cameraNode;
    double lt;

    public:
    VoxelProceduralSceneController(std::shared_ptr<GLSWindow> window);
    virtual ~VoxelProceduralSceneController();

    virtual void makeScene();
    virtual void update();
};
