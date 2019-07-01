
#include "GLSWindow.hpp"

static void _keyCallBack(GLFWwindow *w, int k, int s, int a, int m) {
    GLSWindow *glswin = static_cast<GLSWindow*>(glfwGetWindowUserPointer(w));
    if (glswin == NULL)
        std::cout << "No input handle for event" << std::endl;
    else
        glswin->keyCallBack(k, s, a, m);
}

static void _windowCloseCallBack(GLFWwindow *w) {
    GLSWindow *glswin = static_cast<GLSWindow*>(glfwGetWindowUserPointer(w));
    if (glswin != NULL)
        glswin->close();
}

static void _windowResizeCallBack(GLFWwindow *w, int width, int height) {
    GLSWindow *glswin = static_cast<GLSWindow*>(glfwGetWindowUserPointer(w));
    if (glswin != NULL)
        glswin->updateSize();
    (void)width;
    (void)height;
}

int GLSWindow::_windowCount = 0;
void GLSWindow::_incrementWindowCount() {
    if (_windowCount++ == 0) {
        GLS::glsInit();
    }
}

void GLSWindow::_decrementWindowCount() {
    if (--_windowCount == 0) {
        GLS::glsDeinit();
    }
}

GLSWindow::GLSWindow(AppEnv *env, glm::vec2 size, std::string title) :
_env(env),
_postProcessShaderProgram(nullptr),
_framebuffer(nullptr) {

    _width = size.x;
    _height = size.y;
    _glfwWindow = glfwCreateWindow(_width, _height, title.c_str(), NULL, NULL);
    if (_glfwWindow == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        throw std::exception();
    }
    glfwMakeContextCurrent(_glfwWindow);
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_MULTISAMPLE);
    glfwGetFramebufferSize(_glfwWindow, &_bufferWidth, &_bufferHeight);

    try {
        _incrementWindowCount();
    } catch (GLS::ShaderBuildingException& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << e.infoLog << std::endl;
        throw std::exception();
    } catch (std::exception& e) {
        std::cerr << "Exception during GLS initialisation" << std::endl;
        std::cerr << e.what();
        throw std::exception();
    }

    _scene = std::make_shared<GLS::Scene>(glm::vec2(_bufferWidth, _bufferHeight));

    glfwSetWindowUserPointer(_glfwWindow, this);
    glfwSetKeyCallback(_glfwWindow, _keyCallBack);
    glfwSetWindowCloseCallback(_glfwWindow, _windowCloseCallBack);
    glfwSetWindowSizeCallback(_glfwWindow, _windowResizeCallBack);
    glfwSwapInterval(1);

    _elapsedTime = glfwGetTime();
    _deltaTime = 0;
}

GLSWindow::~GLSWindow() {
    if (_glfwWindow != NULL)
        glfwWindowShouldClose(_glfwWindow);
    _decrementWindowCount();
}

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
    double newTime = glfwGetTime();
    _deltaTime = newTime - _elapsedTime;
    _elapsedTime = newTime;
}

void GLSWindow::keyCallBack(int key, int scan, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE)
        close();
    if (!_controller.expired())
        _controller.lock()->keyCallBack(key, scan, action, mods);
}

glm::vec2 GLSWindow::mousePosition() const {
    return _mousePosition;
}

glm::vec2 GLSWindow::mouseContextPosition() const {
    return glm::vec2(1, -1) * _mousePosition / glm::vec2(_width / 2, _height / 2) + glm::vec2(-1, 1);
}

bool GLSWindow::keyPressed(int key) const {
    if (_glfwWindow == NULL)
        return false;
    return glfwGetKey(_glfwWindow, key) == GLFW_PRESS;
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
    _scene->updateAnimations(_deltaTime);
    if (!_controller.expired())
        _controller.lock()->update();

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

    glfwSwapBuffers(_glfwWindow);
}
