
#pragma once

#include "AppEnv.hpp"

#include "BigChunk.hpp"
#include "DynamicWorld.hpp"
#include "ProceduralWorldGenerator.hpp"



int* initNoise(unsigned int seed);
double smoothNoise(double x, double y, double z);
double linearNoise(double x, double y, double z);

class DynamicWorld;
class VoxelProceduralSceneController: public ISceneController {
    std::shared_ptr<GLS::Node> worldNode;
    std::shared_ptr<GLS::Node> cameraNode;
    double lt;
	std::shared_ptr<DynamicWorld> _dynamicWorld;

    public:
    VoxelProceduralSceneController(std::shared_ptr<GLSWindow> window);
    virtual ~VoxelProceduralSceneController();

    virtual void makeScene();
    virtual void update();
    virtual void keyCallBack(int key, int scancode, int action, int mods);
};
