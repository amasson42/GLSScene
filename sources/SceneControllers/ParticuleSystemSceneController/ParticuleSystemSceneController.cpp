
#include "AppEnv.hpp"

ParticuleSystemSceneController::ParticuleSystemSceneController(std::shared_ptr<GLSWindow> window) :
ISceneController(window) {

}

ParticuleSystemSceneController::~ParticuleSystemSceneController() {
    
}

void ParticuleSystemSceneController::update() {
    ISceneController::update();
    if (!mustUpdate)
        return;
    if (_window.expired())
        return;
    std::shared_ptr<GLSWindow> win = _window.lock();

    glm::vec2 windowMousePos = win->mouseContextPosition();

    if (!particleSystem.expired()) {
        std::shared_ptr<GLS::ParticleSystem> ps = particleSystem.lock();
        glm::vec3 gc(0);
        if (cameraNode != nullptr) {
            float aspect = static_cast<float>(win->size().x) / static_cast<float>(win->size().y);
            gc = glm::vec3(cameraNode->getWorldTransformMatrix() * glm::vec4(aspect * 10 * windowMousePos.x, 10 * windowMousePos.y, -10, 1));
        }
        ps->getAnimationKernel()->setArgument(3, gc.x, gc.y, gc.z);
        if (gravityCenterNode != nullptr) {
            gravityCenterNode->transform().setPosition(gc);
        }

        if (win->keyPressed(GLFW_KEY_T)) {
            ps->setTexture(nullptr);
        }
        if (win->keyPressed(GLFW_KEY_Y)) {
            ps->setTexture(particleTexture);
        }
    }
}

void ParticuleSystemSceneController::makeScene() {
    if (_window.expired())
        return;
    GLS::Scene& scene(*_scene);
    AppEnv *env = _window.lock()->getAppEnvPtr();

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

    auto pointlightNode = std::make_shared<GLS::Node>();
    auto pointlight = std::make_shared<GLS::Light>();
    pointlight->type = GLS::light_point;
    pointlight->color = glm::vec3(1);
    pointlightNode->setLight(pointlight);
    cameraNode->addChildNode(pointlightNode);

    GLS::ParticleSystemProperties psProperties;
    std::shared_ptr<std::string> kernelFilename = env->getArgument("-file");
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

    std::shared_ptr<std::string> textureFilename = env->getArgument("-image");
    if (textureFilename != nullptr) {
        particleTexture = std::make_shared<GLS::Texture>(*textureFilename);
    }

    try {
        std::shared_ptr<GLS::Node> particleNode = std::make_shared<GLS::Node>();
        particleNode->setName("particleNode");
        std::shared_ptr<GLS::ParticleSystem> ps = std::make_shared<GLS::ParticleSystem>(psProperties);
        ps->setTexture(particleTexture);
        ps->generateBuffers();
        ps->initAnimation();
        particleNode->addRenderable(ps);
        scene.rootNode()->addChildNode(particleNode);
        particleSystem = ps;
        particleNode->addAnimatable(ps);
    } catch (CLD::GPUDevice::BuildProgramException& e) {
        std::cout << e.what() << std::endl;
    }

    gravityCenterNode = std::make_shared<GLS::Node>();
    gravityCenterNode->addRenderable(GLS::Mesh::sphere(0.15));
    scene.rootNode()->addChildNode(gravityCenterNode);

}

void ParticuleSystemSceneController::keyCallBack(int k, int s, int a, int m) {
    (void)s;
    (void)a;
    (void)m;
    if (k == GLFW_KEY_P) {
        auto particuleNode = scene()->rootNode()->childNodeNamed("particleNode", true);
        if (particuleNode != nullptr) {
            if (particuleNode->animatables().empty() == false)
                particuleNode->removeAnimatable(0);
        }
    }
    if (k == GLFW_KEY_O) {
        auto particuleNode = scene()->rootNode()->childNodeNamed("particleNode", true);
        if (particuleNode != nullptr) {
            particuleNode->addAnimatable(particleSystem.lock());
        }
    }
}
