
#include "ISceneController.hpp"

ISceneController::ISceneController(std::shared_ptr<GLSWindow> window) :
    _window(window),
    _scene(window->scene()), 
	_cameraMouseControlEnabled(false),
	_lastMousePosition(0),
	_cameraEulerAngles(0, 0) {
    mustUpdate = true;
	cameraMoveSpeed = 1;
    playSpeed = 1;
}

ISceneController::~ISceneController() {
    _scene = nullptr;
}

std::shared_ptr<GLS::Scene> ISceneController::scene() {
    return _scene;
}

nanogui::Screen *ISceneController::nanoguiScreen() {
    if (_window.expired())
        return nullptr;
    else
        return _window.lock()->nanoguiScreen();
}

void ISceneController::update() {
    if (_window.expired()) {
        std::cout << "windows out" << std::endl;
        return;
    }
    std::shared_ptr<GLSWindow> win = _window.lock();

    if (!_scene->cameraNode().expired()) {
        GLS::Node& cam(*_scene->cameraNode().lock());

        float cameraSpeed = cameraMoveSpeed * win->deltaTime();
        glm::mat4 cameraMat = cam.getWorldTransformMatrix();
        glm::vec3 cameraFront = glm::vec3(cameraMat * glm::vec4(0, 0, -1, 0));
        glm::vec3 cameraRight = glm::vec3(cameraMat * glm::vec4(1, 0, 0, 0));
        glm::vec3 cameraUp = glm::vec3(cameraMat * glm::vec4(0, 1, 0, 0));

        if (win->keyPressed(GLFW_KEY_W))
            cam.transform().moveBy(cameraSpeed * cameraFront);
        if (win->keyPressed(GLFW_KEY_S))
            cam.transform().moveBy(-cameraSpeed * cameraFront);
        if (win->keyPressed(GLFW_KEY_A))
            cam.transform().moveBy(-cameraSpeed * cameraRight);
        if (win->keyPressed(GLFW_KEY_D))
            cam.transform().moveBy(cameraSpeed * cameraRight);
        if (win->keyPressed(GLFW_KEY_E))
            cam.transform().moveBy(cameraSpeed * cameraUp);
        if (win->keyPressed(GLFW_KEY_Q))
            cam.transform().moveBy(-cameraSpeed * cameraUp);

        float cameraRotateSpeed = 3.0 * win->deltaTime();
        float cameraMouseRotateSpeed = 20.0 * win->deltaTime();
        if (win->keyPressed(GLFW_KEY_LEFT))
            _cameraEulerAngles.y += cameraRotateSpeed;
        else if (win->keyPressed(GLFW_KEY_RIGHT))
            _cameraEulerAngles.y -= cameraRotateSpeed;
        else if (win->keyPressed(GLFW_KEY_UP))
            _cameraEulerAngles.x -= cameraRotateSpeed;
        else if (win->keyPressed(GLFW_KEY_DOWN))
            _cameraEulerAngles.x += cameraRotateSpeed;

		if (_cameraMouseControlEnabled) {
			glm::vec2 mousePosition = win->mouseContextPosition();
			float xOffset = mousePosition.x - _lastMousePosition.x;
			float yOffset = mousePosition.y - _lastMousePosition.y;

			_lastMousePosition = mousePosition;

			xOffset *= cameraMouseRotateSpeed;
			yOffset *= cameraMouseRotateSpeed;

			_cameraEulerAngles.y -= xOffset;
			_cameraEulerAngles.x += yOffset;

			_cameraEulerAngles.x = glm::clamp(_cameraEulerAngles.x, -1.60f, 1.6f);
		}

		cam.transform().setEulerAngles(_cameraEulerAngles.x, _cameraEulerAngles.y, 0);
    }

    if (win->keyPressed(GLFW_KEY_P)) {
        mustUpdate = false;
    }
    if (win->keyPressed(GLFW_KEY_O)) {
        mustUpdate = true;
    }
    if (mustUpdate) {
        _scene->updateAnimations(playSpeed * win->deltaTime());
    }
}

void ISceneController::resizeWindowCallBack(glm::vec2 newSize) {
    (void)newSize;
    if (!_scene->cameraNode().expired()) {
        std::shared_ptr<GLS::Node> camNode = _scene->cameraNode().lock();
        if (camNode->camera() != nullptr) {
            camNode->camera()->aspect = _scene->getAspect();
        }
    }
}

void ISceneController::keyCallBack(int key, int scancode, int action, int mods) {
    (void)key;
    (void)scancode;
    (void)action;
    (void)mods;
    // std::cout << "{key: " << key << ", scancode: " << scancode
    //     << ", action: " << action << ", mods: " << mods << "}" << std::endl;
}

void ISceneController::scrollCallBack(double x, double y) {
    (void)x;
    (void)y;
}

void ISceneController::mouseButtonCallBack(int button, int action, int modifiers) {
    (void)button;
    (void)action;
    (void)modifiers;
}

void ISceneController::closeCallback() {}

void ISceneController::setCameraMouseControl(bool enabled) {
	_cameraMouseControlEnabled = enabled;

	if (_window.expired()) {
        std::cout << "windos out" << std::endl;
        return;
    }
    std::shared_ptr<GLSWindow> win = _window.lock();

	if (enabled) {
		_lastMousePosition = win->mouseContextPosition();
	}
}
