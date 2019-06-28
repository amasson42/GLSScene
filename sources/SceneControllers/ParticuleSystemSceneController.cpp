
#include "SceneController.hpp"

ParticuleSystemSceneController::ParticuleSystemSceneController(AppEnv *e) :
ISceneController(e) {

}

ParticuleSystemSceneController::~ParticuleSystemSceneController() {
    
}

void ParticuleSystemSceneController::update() {
    ISceneController::update();
    if (!mustUpdate)
        return;
    double dt = env->deltaTime;

    glm::vec2 windowMousePos = env->mouseContextPosition();

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

        if (glfwGetKey(env->window, GLFW_KEY_T) == GLFW_PRESS) {
            ps->setTexture(nullptr);
        }
        if (glfwGetKey(env->window, GLFW_KEY_Y) == GLFW_PRESS) {
            ps->setTexture(particleTexture);
        }
    }
}

void ParticuleSystemSceneController::makeScene() {
    GLS::Scene& scene(*env->scene);

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
    std::shared_ptr<std::string> kernelFilename = env->getArgument("-kernel");
    if (kernelFilename != nullptr) {
        std::ifstream file(*kernelFilename);
        file.seekg(0, file.end);
        size_t length = file.tellg();
        file.seekg(0, file.beg);
        char *buffer = new char[length + 1];
        file.read(buffer, length);
        buffer[length] = '\0';
        psProperties.kernelSource = buffer;
        delete[] buffer;
    }

    std::shared_ptr<std::string> particleCount = env->getArgument("-count");
    if (particleCount != nullptr) {
        psProperties.count = atoi(particleCount->c_str());
    }

    std::shared_ptr<std::string> textureFilename = env->getArgument("-texture");
    if (textureFilename != nullptr) {
        particleTexture = std::make_shared<GLS::Texture>(*textureFilename);
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
