
#include "GLScene.hpp"

static std::weak_ptr<GLS::ParticleSystem> particleSystem;
static std::shared_ptr<GLS::Node> cameraNode = nullptr;
static std::shared_ptr<GLS::Node> gravityCenterNode = nullptr;

extern glm::vec2 windowMousePos;

void updateSceneParticuleSystem(double et, double dt) {
    (void)et;

    if (!particleSystem.expired()) {
        std::shared_ptr<GLS::ParticleSystem> ps = particleSystem.lock();
        ps->animateWithDeltaTime(dt);
        glm::vec3 gc(0);
        if (cameraNode != nullptr) {
            glm::mat4 mvp = glm::inverse(cameraNode->getWorldTransformMatrix());
            if (cameraNode->camera() != nullptr)
                mvp = cameraNode->camera()->projectionMatrix() * mvp;
            gc = glm::vec3(glm::inverse(mvp) * glm::vec4(2, 2, 1, 1) * glm::vec4(windowMousePos.x, windowMousePos.y, 0.2, 1));
        }
        ps->getAnimationKernel()->setArgument(3, gc.x, gc.y, gc.z);
        if (gravityCenterNode != nullptr)
            gravityCenterNode->transform().setPosition(gc);
    }
}

void loadSceneParticuleSystem(GLS::Scene& scene, const std::vector<std::string>& args) {

    (void)args;

    cameraNode = std::make_shared<GLS::Node>();
    {
        std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
        camera->aspect = (scene.getAspect());
        camera->farZ = 50;
        cameraNode->setCamera(camera);
    }
    cameraNode->transform().moveBy(0, 2, 5);
    scene.setCameraNode(cameraNode);
    scene.rootNode()->addChildNode(cameraNode);

    GLS::ParticleSystemProperties psProperties;
    std::cout << "standard particle kernel:" << std::endl << psProperties.kernelSource << std::endl;
    if (args.size() >= 1) {
        if (args[0] != "_") {
            std::ifstream file(args[0]);
            file.seekg(0, file.end);
            size_t length = file.tellg();
            file.seekg(0, file.beg);
            char *buffer = new char[length + 1];
            file.read(buffer, length);
            buffer[length] = '\0';
            psProperties.kernelSource = buffer;
            delete[] buffer;
        }
    }
    if (args.size() >= 2) {
        if (args[1] != "_")
            psProperties.count = atoi(args[1].c_str());
    }
    if (args.size() >= 3) {
        if (args[2] != "_") {
            std::shared_ptr<GLS::Texture> particleTexture = std::make_shared<GLS::Texture>(args[2]);
        }
    }

    try {
        std::shared_ptr<GLS::Node> particleNode = std::make_shared<GLS::Node>();
        std::shared_ptr<GLS::ParticleSystem> ps = std::make_shared<GLS::ParticleSystem>(psProperties);
        ps->generateBuffers();
        ps->initAnimation();
        particleNode->addRenderable(ps);
        scene.rootNode()->addChildNode(particleNode);
        particleSystem = ps;
    } catch (CLD::GPUDevice::BuildProgramException& e) {
        std::cout << e.what() << std::endl;
    }

    gravityCenterNode = std::make_shared<GLS::Node>();
    gravityCenterNode->addRenderable(GLS::Mesh::sphere(0.1));
    scene.rootNode()->addChildNode(gravityCenterNode);

}
