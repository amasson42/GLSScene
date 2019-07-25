
#pragma once

#include "AppEnv.hpp"

class ShadowSceneController: public ISceneController {
    std::shared_ptr<GLS::Node> bigCubeNode;
    std::shared_ptr<GLS::Node> lightPivotNode;
    std::shared_ptr<GLS::Node> cubesPivotNode;
    std::shared_ptr<GLS::InstancedMesh> instancedMeshFloater;
    std::weak_ptr<GLS::ParticleSystem> particleSystem;

    public:
    ShadowSceneController(std::shared_ptr<GLSWindow> window);
    virtual ~ShadowSceneController();

    virtual void makeScene();
    virtual void update();
    virtual void keyCallBack(int key, int scancode, int action, int mods);
};
