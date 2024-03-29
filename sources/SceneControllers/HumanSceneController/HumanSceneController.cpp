

// float mix(float a, float b, double t) {
//     return glm::mix<float, double>(a, b, t);
//     // return ((b - a) * t + a);
// }

#include "AppEnv.hpp"

class MeshDebugAxes : public GLS::IRenderable {

    static std::array<std::shared_ptr<GLS::Mesh>, 3> _axes;
    glm::mat4 _scaler;

    public:

    MeshDebugAxes(glm::vec3 scale = glm::vec3(1)) {
        if (_axes[0] == nullptr) {
            _axes[0] = GLS::Mesh::thinLine(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
            _axes[0]->setMaterial(std::make_shared<GLS::Material>());
            _axes[0]->getMaterial()->diffuse = glm::vec3(1, 0, 0);
        }
        if (_axes[1] == nullptr) {
            _axes[1] = GLS::Mesh::thinLine(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
            _axes[1]->setMaterial(std::make_shared<GLS::Material>());
            _axes[1]->getMaterial()->diffuse = glm::vec3(0, 1, 0);
        }
        if (_axes[2] == nullptr) {
            _axes[2] = GLS::Mesh::thinLine(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
            _axes[2]->setMaterial(std::make_shared<GLS::Material>());
            _axes[2]->getMaterial()->diffuse = glm::vec3(0, 0, 1);
        }
        _scaler = glm::scale(glm::mat4(1), scale);
    }

    virtual void renderInContext(GLS::Scene& scene, const GLS::RenderUniforms& uniforms) {
        GLS::RenderUniforms uniformsScaled = uniforms;
        uniformsScaled.model *= _scaler;
        _axes[0]->renderInContext(scene, uniformsScaled);
        _axes[1]->renderInContext(scene, uniformsScaled);
        _axes[2]->renderInContext(scene, uniformsScaled);
    }

};

std::array<std::shared_ptr<GLS::Mesh>, 3> MeshDebugAxes::_axes = {nullptr, nullptr, nullptr};

HumanSceneController::HumanSceneController(std::shared_ptr<GLSWindow> window) :
ISceneController(window),
_mainNode(nullptr),
_toolsWindow(nullptr)
 {

}

HumanSceneController::~HumanSceneController() {
    
}

void addRenderableToNodeHierarchy(T_Node node, std::shared_ptr<GLS::IRenderable> mesh) {
    node->addRenderable(mesh);
    for (size_t i = 0; i < node->childNodes().size(); i++) {
        addRenderableToNodeHierarchy(node->childNodeAt(i), mesh);
    }
}

static void _createCameraAndLights(GLS::Scene& scene);
static std::shared_ptr<GLS::Node> _createAnimationModel(GLS::Scene& scene, AppEnv* env);
static void _createGround(GLS::Scene& scene, AppEnv* env);

void HumanSceneController::makeScene() {
    if (_window.expired())
        return;
    AppEnv *env = _window.lock()->getAppEnvPtr();
    GLS::Scene& scene(*_scene);

    _createCameraAndLights(scene);
    _mainNode = _createAnimationModel(scene, env);
    if (_mainNode == nullptr) {
        std::shared_ptr<GLS::Node> tentacle = generateTentacle();
        T_Node offseter = newNode("offseter");
        scene.rootNode()->addChildNode(offseter);
        offseter->addChildNode(tentacle);
        addRenderableToNodeHierarchy(tentacle, std::make_shared<MeshDebugAxes>());
        _mainNode = offseter;
    }
    _createGround(scene, env);

    _generateUI();

    mustUpdate = true;
}

static void _createCameraAndLights(GLS::Scene& scene) {
    T_Node cameraNode = newNode();
    cameraNode->setName("cameraNode");
    {
        std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
        camera->aspect = (scene.getAspect());
        camera->farZ = (25.0);
        cameraNode->setCamera(camera);
    }
    cameraNode->transform().moveBy(0, 3, 5);
    std::cout << "camera at: " << cameraNode->transform() << std::endl;
    scene.setCameraNode(cameraNode);
    scene.rootNode()->addChildNode(cameraNode);

    std::shared_ptr<GLS::Light> followLight = std::make_shared<GLS::Light>();
    T_Node followLightNode = newNode("followLightNode");
    followLightNode->transform().setPosition(glm::vec3(0, 0, 0));
    followLightNode->transform().setEulerAngles(0, 0, 0);
    followLight->type = (GLS::light_spot);
    followLight->angle *= 1.0;
    followLightNode->setLight(followLight);
    cameraNode->addChildNode(followLightNode);

    std::shared_ptr<GLS::Light> light = std::make_shared<GLS::Light>();
    T_Node lightNode = newNode("lightNode");
    lightNode->transform().setPosition(glm::vec3(2, 5, 3));
    lightNode->transform().setEulerAngles(-1.5, 0, -0.3);
    light->type = (GLS::light_spot);
    light->cast_shadow = true;
    light->angle *= 1.5;
    lightNode->setLight(light);
    scene.rootNode()->addChildNode(lightNode);

    std::shared_ptr<GLS::Light> ambiantLight = std::make_shared<GLS::Light>();
    T_Node ambiantLightNode = newNode("ambiantLightNode");
    ambiantLight->type = (GLS::light_ambiant);
    ambiantLightNode->setLight(ambiantLight);
    scene.rootNode()->addChildNode(ambiantLightNode);
}

static std::shared_ptr<GLS::Node> _createAnimationModel(GLS::Scene& scene, AppEnv *env) {
    std::shared_ptr<std::string> animationFilenamePtr = env->getArgument("-file");
    if (animationFilenamePtr != nullptr) {
        std::string animationFilename = *animationFilenamePtr;

        T_Node offseter = newNode();
        offseter->setName("offseter");
        offseter->transform().moveBy(0, 0, 0);
        auto scalerIt = std::find(env->args.begin(), env->args.end(), "-scale");
        if (scalerIt != env->args.end()) {
            float scaler = std::stof(*(++scalerIt));
            offseter->transform().setScale(glm::vec3(scaler));
        }
        scene.rootNode()->addChildNode(offseter);
        T_Node animNode = GLS::Node::loadFromFile(animationFilename);
        animNode->setName("animated");
        offseter->addChildNode(animNode);

        std::shared_ptr<GLS::IRenderable> cubeMesh = std::make_shared<MeshDebugAxes>(glm::vec3(0.1, 0.1, 0.1));
        offseter->addRenderable(cubeMesh);
        addRenderableToNodeHierarchy(animNode, cubeMesh);
        if (animNode->hasAnimatable()) {
            std::shared_ptr<std::string> animationNamePtr = env->getArgument("-animation");
            if (animationNamePtr != nullptr) {
                animNode->getAnimatable<GLS::Skeleton>()->setAnimationBlend(*animationNamePtr, 0, 1);
            } else {
                animNode->initAnimation();
            }
            std::cout << "animations: " << std::endl;
            auto skeleton = animNode->getAnimatable<GLS::Skeleton>();
            for (size_t i = 0; i < skeleton->animationNames().size(); i++) {
                std::cout << "  " << skeleton->animationNames()[i] << std::endl;
            }
        }
        return offseter;
    } else {
        return nullptr;
    }
}

static void _createGround(GLS::Scene& scene, AppEnv *env) {
    // create ground plane
    std::shared_ptr<std::string> groundFileNamePtr = env->getArgument("-image");
    if (groundFileNamePtr != nullptr) {
        std::shared_ptr<GLS::Texture> texture = std::make_shared<GLS::Texture>(groundFileNamePtr->c_str());
        texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        float scaleFactor = 10.0 / (float)std::max(texture->width(), texture->height());

        std::shared_ptr<GLS::Material> planeMaterial = std::make_shared<GLS::Material>();

        std::shared_ptr<GLS::Mesh> planeMesh = GLS::Mesh::plane(texture->width(), texture->height());
        planeMaterial->texture_diffuse = texture;
        planeMesh->setMaterial(planeMaterial);

        T_Node plane = newNode("ground_plane");
        plane->addRenderable(planeMesh);
        plane->transform().setEulerAngles(-M_PI / 2, M_PI, 0);

        plane->transform().setScale(glm::vec3(scaleFactor, scaleFactor, scaleFactor));
        scene.rootNode()->addChildNode(plane);
    }
}

void HumanSceneController::update() {
    ISceneController::update();
    if (!mustUpdate)
        return;
    float currentTime = _window.expired() ? 0 : _window.lock()->elapsedTime();
    float deltaTime = _window.expired() ? 0 : _window.lock()->deltaTime();

    (void)currentTime;
    (void)deltaTime;
}

void HumanSceneController::keyCallBack(int key, int scancode, int action, int mods) {
    ISceneController::keyCallBack(key, scancode, action, mods);

}
