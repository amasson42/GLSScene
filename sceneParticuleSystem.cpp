
#ifdef __APPLE__
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include "GLScene.hpp"
#include <GLFW/glfw3.h>

static std::weak_ptr<GLS::ParticleSystem> particleSystem;
static std::shared_ptr<GLS::Node> cameraNode = nullptr;
static std::shared_ptr<GLS::Node> gravityCenterNode = nullptr;
static std::shared_ptr<GLS::Texture> particleTexture = nullptr;

extern GLFWwindow *window;
extern glm::vec2 windowMousePos;

void updateSceneParticuleSystem(double et, double dt) {
    (void)et;

    if (!particleSystem.expired()) {
        std::shared_ptr<GLS::ParticleSystem> ps = particleSystem.lock();
        ps->animateWithDeltaTime(dt);
        glm::vec3 gc(0);
        if (cameraNode != nullptr) {
            gc = glm::vec3(cameraNode->getWorldTransformMatrix() * glm::vec4(windowMousePos.x, windowMousePos.y, -10, 1));
        }
        ps->getAnimationKernel()->setArgument(3, gc.x, gc.y, gc.z);
        if (gravityCenterNode != nullptr)
            gravityCenterNode->transform().setPosition(gc);

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            ps->setTexture(nullptr);
        }
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
            ps->setTexture(particleTexture);
        }
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
            particleTexture = std::make_shared<GLS::Texture>(args[2]);
        }
    }

    try {
        std::shared_ptr<GLS::Node> particleNode = std::make_shared<GLS::Node>();
        std::shared_ptr<GLS::ParticleSystem> ps = std::make_shared<GLS::ParticleSystem>(psProperties);
        ps->setTexture(particleTexture);
        ps->generateBuffers();
        ps->initAnimation();
        particleNode->addRenderable(ps);
        scene.rootNode()->addChildNode(particleNode);
        particleSystem = ps;
    } catch (CLD::GPUDevice::BuildProgramException& e) {
        std::cout << e.what() << std::endl;
    }

    gravityCenterNode = std::make_shared<GLS::Node>();
    gravityCenterNode->addRenderable(GLS::Mesh::sphere(0.05));
    scene.rootNode()->addChildNode(gravityCenterNode);

    GLS::Particle();

}
