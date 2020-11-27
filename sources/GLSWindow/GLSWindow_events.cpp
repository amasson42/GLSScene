
#include "GLSWindow.hpp"

void GLSWindow::close() {
    glfwDestroyWindow(_glfwWindow);
    _glfwWindow = NULL;
}

void GLSWindow::updateSize() {
    if (_glfwWindow == NULL)
        return;
    int width, height;
    glfwGetWindowSize(_glfwWindow, &width, &height);
    if (width != _width || _height != height) {
        _width = width;
        _height = height;
        glfwGetFramebufferSize(_glfwWindow, &_bufferWidth, &_bufferHeight);
        glm::vec2 newSize(_bufferWidth, _bufferHeight);
        _scene->setSize(newSize);
        if (!_controller.expired())
            _controller.lock()->resizeWindowCallBack(newSize);
        _framebuffer = nullptr;
        _createEffectFramebuffer();
    }
}

void GLSWindow::updateEvents() {
    if (_glfwWindow == NULL)
        return;
    double mouseX, mouseY;
    glfwGetCursorPos(_glfwWindow, &mouseX, &mouseY);
    _mousePosition = glm::vec2(mouseX, mouseY);
    if (_postProcessShaderProgram != nullptr) {
        glm::vec2 devicePos = mouseContextPosition();
        _postProcessShaderProgram->use();
        glUniform2f(_postProcessShaderProgram->getLocation("u_mouse_position"), devicePos.x, devicePos.y);
        glUniform1f(_postProcessShaderProgram->getLocation("u_time"), _elapsedTime);
    }
    timefloat newTime = static_cast<timefloat>(glfwGetTime());
    _deltaTime = newTime - _elapsedTime;
    _elapsedTime = newTime;
}

void GLSWindow::keyCallBack(int key, int scan, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE) {
        closeCallback();
	}
		
    if (!_controller.expired())
        _controller.lock()->keyCallBack(key, scan, action, mods);
}

void GLSWindow::scrollCallBack(double x, double y) {
    if (!_controller.expired())
        _controller.lock()->scrollCallBack(x, y);
}

void GLSWindow::mouseButtonCallBack(int button, int action, int modifier) {
    if (!_controller.expired())
        _controller.lock()->mouseButtonCallBack(button, action, modifier);
}

void GLSWindow::closeCallback() {
    if (!_controller.expired())
        _controller.lock()->closeCallback();
	close();
}

glm::vec2 GLSWindow::mousePosition() const {
    return _mousePosition;
}

glm::vec2 GLSWindow::mouseContextPosition() const {
    return glm::vec2(1, -1) * _mousePosition / glm::vec2(_width / 2, _height / 2) + glm::vec2(-1, 1);
}

void GLSWindow::setMouseInputMode(int mode) {
	glfwSetInputMode(_glfwWindow, GLFW_CURSOR, mode);
}

bool GLSWindow::keyPressed(int key) const {
    if (_glfwWindow == NULL)
        return false;
    return glfwGetKey(_glfwWindow, key) == GLFW_PRESS;
}
