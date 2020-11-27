

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
    std::shared_ptr<std::string> animationFilename = env->getArgument("-file");
    if (animationFilename != nullptr) {
        T_Node animNode = newNode();
        animNode->loadFromFile(*animationFilename);
        scene.rootNode()->addChildNode(animNode);
        animNode->sendToFlux(std::cout, "");
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


}
