
#include "GLSWindow.hpp"

int GLSWindow::_windowCount = 0;
void GLSWindow::_incrementWindowCount() {
    if (_windowCount++ == 0) {
		#ifdef WIN32
        GLS::glsInit(&glfwGetProcAddress);
		#else
        GLS::glsInit(nullptr);
		#endif
    }
}

void GLSWindow::_decrementWindowCount() {
    if (--_windowCount == 0) {
        GLS::glsDeinit();
    }
}

GLSWindow::GLSWindow(AppEnv *env, glm::vec2 size, std::string title, bool fullScreen, GLSWindow *shareContext) :
_env(env),
_postProcessShaderProgram(nullptr),
_framebuffer(nullptr) {

    glfwWindowHint(GLFW_SAMPLES, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);

    _width = size.x;
    _height = size.y;
    _glfwWindow = glfwCreateWindow(_width, _height, title.c_str(), fullScreen ? glfwGetPrimaryMonitor() : NULL, shareContext != nullptr ? shareContext->_glfwWindow : NULL);
    if (_glfwWindow == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        throw std::runtime_error("Can't initiliaze GLFW");
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
        throw e;
    } catch (std::exception& e) {
        std::cerr << "Exception during GLS initialisation" << std::endl;
        std::cerr << e.what();
        throw e;
    }


    glfwSetWindowUserPointer(_glfwWindow, this);
    

    glfwSetCursorPosCallback(_glfwWindow, [](GLFWwindow* win, double x, double y) {
		GLSWindow* window = static_cast<GLSWindow*>(glfwGetWindowUserPointer(win));

		window->nanoguiScreen()->cursor_pos_callback_event(x, y);
	});

    glfwSetMouseButtonCallback(_glfwWindow, [](GLFWwindow* win, int button, int action, int modifiers) {
		GLSWindow* window = static_cast<GLSWindow*>(glfwGetWindowUserPointer(win));

        window->nanoguiScreen()->mouse_button_callback_event(button, action, modifiers);
        // if (!window->nanoguiScreen()->mouse_button_callback_event(button, action, modifiers)) {
        // 	window->mouseButtonCallBack(button, action, modifiers);
	    // }
    });

	glfwSetKeyCallback(_glfwWindow, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
		GLSWindow* window = static_cast<GLSWindow*>(glfwGetWindowUserPointer(win));

		window->keyCallBack(key, scancode, action, mods);
        window->nanoguiScreen()->key_callback_event(key, scancode, action, mods);
	});

	glfwSetCharCallback(_glfwWindow, [](GLFWwindow* win, unsigned int codepoint) {
		GLSWindow* window = static_cast<GLSWindow*>(glfwGetWindowUserPointer(win));

    	window->nanoguiScreen()->char_callback_event(codepoint);
	});

    glfwSetWindowCloseCallback(_glfwWindow, [](GLFWwindow* win) {
        GLSWindow* window = static_cast<GLSWindow*>(glfwGetWindowUserPointer(win));
		window->closeCallback();
		window->close();
    });

    glfwSetWindowSizeCallback(_glfwWindow, [](GLFWwindow* win, int width, int height) {
        GLSWindow* window = static_cast<GLSWindow*>(glfwGetWindowUserPointer(win));
        if (window != NULL)
            window->updateSize();
    	window->nanoguiScreen()->resize_callback_event(width, height);
    });

    glfwSetScrollCallback(_glfwWindow, [](GLFWwindow* win, double x, double y) {
        GLSWindow* window = static_cast<GLSWindow*>(glfwGetWindowUserPointer(win));

        window->nanoguiScreen()->scroll_callback_event(x, y);
        // if (!window->nanoguiScreen()->scroll_callback_event(x, y)) {
        //     window->scrollCallBack(x, y);
		// }
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
