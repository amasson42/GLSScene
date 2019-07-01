
#pragma once

#include "AppEnv.hpp"

class ParticuleSystemSceneController: public ISceneController {
    std::weak_ptr<GLS::ParticleSystem> particleSystem;
    std::shared_ptr<GLS::Node> cameraNode;
    std::shared_ptr<GLS::Node> gravityCenterNode;
    std::shared_ptr<GLS::Texture> particleTexture;

    public:
    ParticuleSystemSceneController(std::shared_ptr<GLSWindow> window);
    virtual ~ParticuleSystemSceneController();

    virtual void makeScene();
    virtual void update();
};
