
#include "GLScene.hpp"

static std::weak_ptr<GLS::ParticleSystem> particleSystem;

void updateSceneParticuleSystem(double et, double dt) {
    (void)et;

    if (!particleSystem.expired()) {
        particleSystem.lock()->animateWithDeltaTime(dt);
    }
}

void loadSceneParticuleSystem(GLS::Scene& scene, const std::vector<std::string>& args) {

    (void)args;

    std::shared_ptr<GLS::Node> cameraNode = std::make_shared<GLS::Node>();
    {
        std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
        camera->aspect = (scene.getAspect());
        cameraNode->setCamera(camera);
    }
    cameraNode->transform().moveBy(0, 2, 5);
    scene.setCameraNode(cameraNode);
    scene.rootNode()->addChildNode(cameraNode);

    std::shared_ptr<GLS::Node> particleNode = std::make_shared<GLS::Node>();
    std::shared_ptr<GLS::ParticleSystem> ps = std::make_shared<GLS::ParticleSystem>();
    ps->generateBuffers();
    ps->initAnimation();
    particleNode->addRenderable(ps);
    scene.rootNode()->addChildNode(particleNode);
    particleSystem = ps;

}
