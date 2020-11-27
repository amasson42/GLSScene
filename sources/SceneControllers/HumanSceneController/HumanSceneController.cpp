

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
        lightNode->transform().setPosition(glm::vec3(3, 5, 3));
        light->type = (GLS::light_point);
        lightNode->setLight(light);
        scene.rootNode()->addChildNode(lightNode);
    }

    // just a nanosuit
    std::shared_ptr<std::string> animationFilename = env->getArgument("-file");
    if (animationFilename != nullptr) {
        T_Node animNode = newNode();
        animNode->loadFromFile(*animationFilename);
        scene.rootNode()->addChildNode(animNode);
    } else {

    }

    GLS::Interpolator<float> myKeys;

    std::cout << myKeys << std::endl;

    myKeys.addKeyValueAt(0.5, 10.0);
    myKeys.addKeyValueAt(0.6, -2.0);
    myKeys.addKeyValueAt(1.0, 1.0);
    myKeys.addKeyValueAt(5.0, 0.0);

    std::cout << myKeys << std::endl;

    for (double i = 0; i < 5.0; i += 0.1) {
        std::cout << "[" << i << "] => " << myKeys.valueAt(i) << std::endl;
    }

    // create ground plane

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
    // float loopTime = fmod(currentTime, hoomanAnimator->duration());
    
}
