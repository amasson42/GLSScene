
#pragma once

#include "AppEnv.hpp"

struct AppEnv;
class ISceneController {

    public:

    AppEnv *env;
    bool mustUpdate;

    ISceneController(AppEnv *e);
    virtual ~ISceneController();

    std::shared_ptr<GLS::Scene> scene();

    virtual void makeScene() = 0;
    virtual void update();
    virtual void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
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
