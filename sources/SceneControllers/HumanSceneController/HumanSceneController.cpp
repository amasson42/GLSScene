

// float mix(float a, float b, double t) {
//     return glm::mix<float, double>(a, b, t);
//     // return ((b - a) * t + a);
// }

#include "AppEnv.hpp"

// struct FloatFrame {
//     float value;

//     FloatFrame(float v = 0) {
//         value = v;
//     }

//     static FloatFrame linearValue(FloatFrame a, FloatFrame b, double t) {
//         return FloatFrame(glm::mix(a.value, b.value, t));
//     }
// };

// std::ostream& operator<<(std::ostream& out, const FloatFrame& frame) {
//     out << frame.value;
//     return out;
// }

HumanSceneController::HumanSceneController(std::shared_ptr<GLSWindow> window) :
ISceneController(window) {

}

HumanSceneController::~HumanSceneController() {
    
}

void addCubeToNode(T_Node node, std::shared_ptr<GLS::Mesh> mesh) {
    node->addRenderable(mesh);
    for (size_t i = 0; i < node->childNodes().size(); i++) {
        addCubeToNode(node->childNodeAt(i), mesh);
    }
}

static void _createCameraAndLights(GLS::Scene& scene);

void HumanSceneController::makeScene() {
    if (_window.expired())
        return;
    AppEnv *env = _window.lock()->getAppEnvPtr();
    GLS::Scene& scene(*_scene);

    _createCameraAndLights(scene);

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

        std::shared_ptr<GLS::Mesh> cubeMesh = GLS::Mesh::cube(0.08, 0.1, 0.08);
        offseter->addRenderable(cubeMesh);
        offseter->addRenderable(cubeMesh);
        offseter->addRenderable(cubeMesh);
        // addCubeToNode(animNode, cubeMesh);
        if (animNode->hasAnimatable()) {
            animNode->initAnimation();
            std::cout << "animations: " << std::endl;
            auto skeleton = animNode->getAnimatable<GLS::Skeleton>();
            for (size_t i = 0; i < skeleton->animationNames().size(); i++) {
                std::cout << "  " << skeleton->animationNames()[i] << std::endl;
            }
        }
    }

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

        T_Node plane = newNode();
        plane->addRenderable(planeMesh);
        plane->transform().setEulerAngles(-M_PI / 2, M_PI, 0);

        plane->transform().setScale(glm::vec3(scaleFactor, scaleFactor, scaleFactor));
        scene.rootNode()->addChildNode(plane);
    }

    // create grass on it

    // create beautiful car

    // create some trees

    // create random shit mesh

    mustUpdate = false;
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
    T_Node followLightNode = newNode();
    followLightNode->setName("followLightNode");
    followLightNode->transform().setPosition(glm::vec3(0, 0, 0));
    followLightNode->transform().setEulerAngles(0, 0, 0);
    followLight->type = (GLS::light_spot);
    followLight->angle *= 1.0;
    followLightNode->setLight(followLight);
    cameraNode->addChildNode(followLightNode);

    std::shared_ptr<GLS::Light> light = std::make_shared<GLS::Light>();
    T_Node lightNode = newNode();
    lightNode->setName("lightNode");
    lightNode->transform().setPosition(glm::vec3(2, 5, 3));
    lightNode->transform().setEulerAngles(-1.5, 0, -0.3);
    light->type = (GLS::light_spot);
    light->cast_shadow = true;
    light->angle *= 1.5;
    lightNode->setLight(light);
    scene.rootNode()->addChildNode(lightNode);
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
