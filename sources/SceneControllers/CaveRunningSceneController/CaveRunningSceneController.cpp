
#include "AppEnv.hpp"

CaveRunningSceneController::CaveRunningSceneController(std::shared_ptr<GLSWindow> window) :
ISceneController(window) {

}

CaveRunningSceneController::~CaveRunningSceneController() {

}

void CaveRunningSceneController::makeScene() {
    if (_window.expired())
        return;
    AppEnv *env = _window.lock()->getAppEnvPtr();
    GLS::Scene& scene(*_scene);

    std::shared_ptr<GLS::Node> cameraNode = std::make_shared<GLS::Node>();
    std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
    camera->aspect = scene.getAspect();
    camera->farZ = 25.0;
    cameraNode->setCamera(camera);
    cameraNode->transform().moveBy(0, 0, 7);
    scene.setCameraNode(cameraNode);
    scene.rootNode()->addChildNode(cameraNode);

    std::shared_ptr<GLS::Light> light = std::make_shared<GLS::Light>();
    light->type = GLS::light_point;
    cameraNode->setLight(light);

    _game = std::make_shared<CaveRunningGame>(scene);
}

void CaveRunningSceneController::update() {
    ISceneController::update();
}

void CaveRunningSceneController::keyCallBack(int key, int scancode, int action, int mods) {
    std::cout << "key-" << key << " scancode-" << scancode << " action-" << action << " mods-" << mods << std::endl;
    switch (key) {
    case '1':
        _game->createNewGame();
        break;
    case '2':
        _game->displayCaveScheme();
        break;
    case '3':
        _game->createRoomNodes(glm::ivec2(0, 0));
        break;
    }
    
}

void CaveRunningSceneController::scrollCallBack(double x, double y) {

}

void CaveRunningSceneController::resizeWindowCallBack(glm::vec2 newSize) {

}

void CaveRunningSceneController::mouseButtonCallBack(int button, int action, int modifiers) {

}

void CaveRunningSceneController::closeCallback() {
    
}