
#include "GLSWindow.hpp"

void GLSWindow::_createEffectFramebuffer() {
    if (_postProcessShaderProgram != nullptr) {
        _framebuffer = std::make_shared<GLS::Framebuffer>(_bufferWidth, _bufferHeight);
        _framebuffer->setProgram(_postProcessShaderProgram);
    } else {
        _framebuffer = nullptr;
    }
}

void GLSWindow::setTitle(std::string title) {
    if (_glfwWindow != NULL)
        glfwSetWindowTitle(_glfwWindow, title.c_str());
}

void GLSWindow::setPostProcessEffect(std::shared_ptr<GLS::ShaderProgram> program) {
    _postProcessShaderProgram = program;
    _framebuffer = nullptr;
    _createEffectFramebuffer();
}

void GLSWindow::setController(std::shared_ptr<ISceneController> controller) {
    _controller = controller;
}

glm::ivec2 GLSWindow::size() const {
    return glm::ivec2(_width, _height);
}

glm::ivec2 GLSWindow::bufferSize() const {
    return glm::ivec2(_bufferWidth, _bufferHeight);
}

bool GLSWindow::isActive() const {
    return _glfwWindow != NULL;
}

std::shared_ptr<GLS::Scene> GLSWindow::scene() {
    return _scene;
}

std::shared_ptr<ISceneController> GLSWindow::controller() {
    return _controller.expired() ? nullptr : _controller.lock();
}

AppEnv *GLSWindow::getAppEnvPtr() const {
    return _env;
}

float GLSWindow::elapsedTime() const {
    return _elapsedTime;
}

float GLSWindow::deltaTime() const {
    return _deltaTime;
}

void GLSWindow::loopOnce() {
    if (_glfwWindow == NULL || glfwWindowShouldClose(_glfwWindow))
        return;
    glfwMakeContextCurrent(_glfwWindow);

    updateEvents();

    if (_framebuffer != nullptr) {
        _scene->renderInContext(_framebuffer);
        _framebuffer->unbind();
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        GLS::RenderUniforms uniforms = _scene->makeUniforms();
        _framebuffer->renderInContext(*_scene, uniforms);
    } else {
        _scene->renderInContext();
    }


    if (!_controller.expired())
        _controller.lock()->update();

    glEnable(GL_DEPTH_TEST);
    _nanoguiScreen->drawContents();
    _nanoguiScreen->drawWidgets();
    glfwSwapBuffers(_glfwWindow);
}

nanogui::Screen *GLSWindow::nanoguiScreen() {
    return _nanoguiScreen;
}

T_Node newNode(std::string name) {
    T_Node node = std::make_shared<GLS::Node>();
    node->setName(name);
    return node;
}
