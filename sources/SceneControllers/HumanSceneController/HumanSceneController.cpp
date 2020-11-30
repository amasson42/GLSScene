

// float mix(float a, float b, double t) {
//     return glm::mix<float, double>(a, b, t);
//     // return ((b - a) * t + a);
// }
#include "GLSSkeleton.hpp"
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

void HumanSceneController::makeScene() {
    if (_window.expired())
        return;
    AppEnv *env = _window.lock()->getAppEnvPtr();
    GLS::Scene& scene(*_scene);

    {// camera and light
        T_Node cameraNode = newNode();
        {
            std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
            camera->aspect = (scene.getAspect());
            camera->farZ = (25.0);
            cameraNode->setCamera(camera);
        }
        cameraNode->transform().moveBy(0, 3, 5);
        scene.setCameraNode(cameraNode);
        scene.rootNode()->addChildNode(cameraNode);

        std::shared_ptr<GLS::Light> light = std::make_shared<GLS::Light>();
        T_Node lightNode = newNode();
        lightNode->transform().setPosition(glm::vec3(0, 0, 0));
        light->type = (GLS::light_spot);
        light->cast_shadow = true;
        light->angle *= 1.5;
        lightNode->setLight(light);
        cameraNode->addChildNode(lightNode);
    }

    // just a nanosuit
    std::shared_ptr<std::string> animationFilenamePtr = env->getArgument("-file");
    std::string animationFilename = animationFilenamePtr == nullptr ? "/Users/giantwow/Documents/static.nosync/3dmodels/glTF-Sample-Models/sourceModels/WalkingLady/WalkingLady.dae" : *animationFilenamePtr;

    T_Node offseter = newNode();
    offseter->transform().moveBy(0, 0, 0);
    auto scalerIt = std::find(env->args.begin(), env->args.end(), "-scale");
    if (scalerIt != env->args.end()) {
        float scaler = std::stof(*(++scalerIt));
        offseter->transform().setScale(glm::vec3(scaler));
    }
    scene.rootNode()->addChildNode(offseter);
    T_Node animNode = GLS::Node::loadFromFile(animationFilename);
    animNode->sendToFlux(std::cout, "~");
    animNode->setName("animated");
    offseter->addChildNode(animNode);

    std::shared_ptr<GLS::Mesh> cubeMesh = GLS::Mesh::cube(0.08, 0.10, 0.08);
    addCubeToNode(animNode, cubeMesh);
    if (animNode->hasSkeleton()) {
        animNode->skeleton()->initAnimation();
        std::cout << "animations: " << std::endl;
        for (int i = 0; i < animNode->skeleton()->animationNames().size(); i++) {
            std::cout << "  " << animNode->skeleton()->animationNames()[i] << std::endl;
        }
    }

    // create ground plane
    T_Node plane = newNode();
    plane->addRenderable(GLS::Mesh::plane(10, 10));
    plane->transform().setEulerAngles(-M_PI / 2, 0, 0);
    scene.rootNode()->addChildNode(plane);

    // create grass on it

    // create beautiful car

    // create some trees

    // create random shit mesh

}

void HumanSceneController::update() {
    ISceneController::update();
    if (!mustUpdate)
        return;
    float currentTime = _window.expired() ? 0 : _window.lock()->elapsedTime();
    float deltaTime = _window.expired() ? 0 : _window.lock()->deltaTime();

    T_Node animateNode = _scene->rootNode()->childNodeNamed("animated", true);
    if (animateNode != nullptr) {
        if (animateNode->hasSkeleton())
            animateNode->skeleton()->animate(deltaTime);
    }

}
