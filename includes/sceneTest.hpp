//
//  sceneTest.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#pragma once

#include <iostream>
#include <sstream>

#ifdef __APPLE__
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include "GLScene.hpp"
#include <vector>

#include <GLFW/glfw3.h>
#include <cmath>

class ISceneController;
struct AppEnv {

    std::vector<std::string> args;
    GLFWwindow *window;

    int windowWidth;
    int windowHeight;

    int windowBufferWidth;
    int windowBufferHeight;

    glm::vec2 mousePosition;

    std::shared_ptr<GLS::Scene> scene;

    bool mustUpdate;
    double currentTime;
    double deltaTime;
    double fpsDisplayCD;

    ISceneController *controller;

    std::shared_ptr<GLS::ShaderProgram> postProcessShaderProgram;

    AppEnv(const std::vector<std::string>& as);

    ~AppEnv();

    glm::vec2 mouseContextPosition() const;
    std::shared_ptr<std::string> getArgument(std::string key) const;

    std::shared_ptr<GLS::Framebuffer> createEffectFramebuffer();
    void loop();
    bool displayFps();
    void checkSize(std::shared_ptr<GLS::Framebuffer> effectFramebuffer);
    void processInput();
};

class ISceneController {

    public:

    AppEnv *env;

    ISceneController(AppEnv *e) : env(e) {}
    virtual ~ISceneController() {}

    std::shared_ptr<GLS::Scene> scene() {
        return env->scene;
    }

    virtual void makeScene() = 0;
    virtual void update() = 0;

};

class TrashSceneController: public ISceneController {
    public:
    TrashSceneController(AppEnv *e);
    virtual ~TrashSceneController();

    virtual void makeScene();
    virtual void update();
};

class Human;
struct HumanAnimationState;

class HumanSceneController: public ISceneController {
    std::shared_ptr<Human> hooman;
    std::shared_ptr<GLS::Animator<HumanAnimationState> > hoomanAnimator;
    public:
    HumanSceneController(AppEnv *e);
    virtual ~HumanSceneController();

    virtual void makeScene();
    virtual void update();
};

class ShadowSceneController: public ISceneController {
    std::shared_ptr<GLS::Node> bigCubeNode;
    std::shared_ptr<GLS::Node> lightPivotNode;
    std::shared_ptr<GLS::Node> cubesPivotNode;
    std::shared_ptr<GLS::InstancedMesh> instancedMeshFloater;
    std::weak_ptr<GLS::ParticleSystem> particleSystem;

    public:
    ShadowSceneController(AppEnv *e);
    virtual ~ShadowSceneController();

    virtual void makeScene();
    virtual void update();
};

class VoxelSceneController: public ISceneController {
    std::shared_ptr<GLS::Node> mainChunkNode;
    std::shared_ptr<GLS::VoxelChunk> mainChunk;
    std::array<std::weak_ptr<GLS::VoxelChunk>, 6> neighbourgsChunks;
    double removeBlockCD;
    int i;

    public:
    VoxelSceneController(AppEnv *e);
    virtual ~VoxelSceneController();

    virtual void makeScene();
    virtual void update();
};

class VoxelWorld;
class VoxelProceduralSceneController: public ISceneController {
    std::shared_ptr<GLS::Node> worldNode;
    std::shared_ptr<GLS::Node> cameraNode;
    double lt;

    public:
    VoxelProceduralSceneController(AppEnv *e);
    virtual ~VoxelProceduralSceneController();

    virtual void makeScene();
    virtual void update();
};

class ParticuleSystemSceneController: public ISceneController {
    std::weak_ptr<GLS::ParticleSystem> particleSystem;
    std::shared_ptr<GLS::Node> cameraNode;
    std::shared_ptr<GLS::Node> gravityCenterNode;
    std::shared_ptr<GLS::Texture> particleTexture;

    public:
    ParticuleSystemSceneController(AppEnv *e);
    virtual ~ParticuleSystemSceneController();

    virtual void makeScene();
    virtual void update();
};

void initNoise(unsigned int seed);
double noise(double x, double y, double z);
