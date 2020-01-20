
#include "AppEnv.hpp"

GrosSceneController::GrosSceneController(std::shared_ptr<GLSWindow> window) :
ISceneController(window) {
}

GrosSceneController::~GrosSceneController() {
    
}

void GrosSceneController::update() {
    ISceneController::update();
	// float deltaTime = _window.lock()->deltaTime();

}

void GrosSceneController::makeScene() {

}
