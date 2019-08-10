
#include "GLSWindow.hpp"

int GLSWindow::_windowCount = 0;
void GLSWindow::_incrementWindowCount() {
    if (_windowCount++ == 0) {
        GLS::glsInit(&glfwGetProcAddress);
    }
}

void GLSWindow::_decrementWindowCount() {
    if (--_windowCount == 0) {
        // GLS::glsDeinit();
    }
}

GLSWindow::GLSWindow(AppEnv *env, glm::vec2 size, std::string title, bool fullScreen, GLSWindow *shareContext) :
_env(env),
_postProcessShaderProgram(nullptr),
_framebuffer(nullptr) {

    _width = size.x;
    _height = size.y;
    _glfwWindow = glfwCreateWindow(_width, _height, title.c_str(), fullScreen ? glfwGetPrimaryMonitor() : NULL, shareContext != nullptr ? shareContext->_glfwWindow : NULL);
    if (_glfwWindow == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        throw std::exception();
    }
    glfwSetWindowSizeLimits(_glfwWindow, 300, 200, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(_glfwWindow);

	_nanoguiScreen = new nanogui::Screen();
	_nanoguiScreen->initialize(_glfwWindow, false);
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


    glfwSetWindowUserPointer(_glfwWindow, this);
    

    glfwSetCursorPosCallback(_glfwWindow, [](GLFWwindow* win, double x, double y) {
		GLSWindow* window = static_cast<GLSWindow*>(glfwGetWindowUserPointer(win));

		window->nanoguiScreen()->cursorPosCallbackEvent(x, y);
	});

    glfwSetMouseButtonCallback(_glfwWindow, [](GLFWwindow* win, int button, int action, int modifiers) {
		GLSWindow* window = static_cast<GLSWindow*>(glfwGetWindowUserPointer(win));

        window->mouseButtonCallBack(button, action, modifiers);
        window->nanoguiScreen()->mouseButtonCallbackEvent(button, action, modifiers);
    });

	glfwSetKeyCallback(_glfwWindow, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
		GLSWindow* window = static_cast<GLSWindow*>(glfwGetWindowUserPointer(win));

		window->keyCallBack(key, scancode, action, mods);
        window->nanoguiScreen()->keyCallbackEvent(key, scancode, action, mods);
	});

	glfwSetCharCallback(_glfwWindow, [](GLFWwindow* win, unsigned int codepoint) {
		GLSWindow* window = static_cast<GLSWindow*>(glfwGetWindowUserPointer(win));

    	window->nanoguiScreen()->charCallbackEvent(codepoint);
	});

    glfwSetWindowCloseCallback(_glfwWindow, [](GLFWwindow* win) {
        GLSWindow* window = static_cast<GLSWindow*>(glfwGetWindowUserPointer(win));
        if (window != NULL)
            window->close();
    });

    glfwSetWindowSizeCallback(_glfwWindow, [](GLFWwindow* win, int width, int height) {
        GLSWindow* window = static_cast<GLSWindow*>(glfwGetWindowUserPointer(win));
        if (window != NULL)
            window->updateSize();
    	window->nanoguiScreen()->resizeCallbackEvent(width, height);
    });

    glfwSetScrollCallback(_glfwWindow, [](GLFWwindow* win, double x, double y) {
        GLSWindow* window = static_cast<GLSWindow*>(glfwGetWindowUserPointer(win));

        window->scrollCallBack(x, y);
        window->nanoguiScreen()->scrollCallbackEvent(x, y);
    });

    glfwSwapInterval(1);

    _scene = std::make_shared<GLS::Scene>(glm::vec2(_bufferWidth, _bufferHeight));
    _elapsedTime = glfwGetTime();
    _deltaTime = 0;
}

GLSWindow::~GLSWindow() {
    if (_glfwWindow != NULL)
        glfwWindowShouldClose(_glfwWindow);
    _decrementWindowCount();
}
