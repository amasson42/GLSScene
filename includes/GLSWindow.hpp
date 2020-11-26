
#pragma once

#define GLFW_INCLUDE_NONE

#ifdef __APPLE__
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
# define GL_SILENCE_DEPRECATION
#endif


#ifdef WIN32
#include <glad/glad.h>
#endif

#include <nanogui/nanogui.h>
#include <GLFW/glfw3.h>
#include "GLScene.hpp"

#include "AppEnv.hpp"

class GLSWindow {

    AppEnv *_env;
    GLFWwindow *_glfwWindow;
    nanogui::Screen *_nanoguiScreen;
    int _width;
    int _height;
    int _bufferWidth;
    int _bufferHeight;

    glm::vec2 _mousePosition;

    std::shared_ptr<GLS::Scene> _scene;
    std::weak_ptr<ISceneController> _controller;

    std::shared_ptr<GLS::ShaderProgram> _postProcessShaderProgram;
    std::shared_ptr<GLS::Framebuffer> _framebuffer;

    float _elapsedTime;
    float _deltaTime;

    void _createEffectFramebuffer();

    static int _windowCount;
    static void _incrementWindowCount();
    static void _decrementWindowCount();

    public:

    GLSWindow(AppEnv *env, glm::vec2 size, std::string title, bool fullScreen = false, GLSWindow *shareContext = nullptr);
    virtual ~GLSWindow();

    void setTitle(std::string title);
    void setPostProcessEffect(std::shared_ptr<GLS::ShaderProgram> program);
    void setController(std::shared_ptr<ISceneController> controller);

    // Getters
    glm::ivec2 size() const;
    glm::ivec2 bufferSize() const;
    bool isActive() const;
    std::shared_ptr<GLS::Scene> scene();
    std::shared_ptr<ISceneController> controller();
    AppEnv *getAppEnvPtr() const;
    nanogui::Screen *nanoguiScreen();

    // Window events
    void close();
    void updateSize();
    void updateEvents();
    void keyCallBack(int key, int scan, int action, int mods);
    void scrollCallBack(double x, double y);
    void mouseButtonCallBack(int button, int action, int modifiers);
	void closeCallback();

    glm::vec2 mousePosition() const;
    glm::vec2 mouseContextPosition() const;

	void setMouseInputMode(int mode);

    bool keyPressed(int key) const;

    float elapsedTime() const;
    float deltaTime() const;

    // GameLoop
    void loopOnce();

};

typedef std::shared_ptr<GLS::Node> T_Node;
T_Node newNode();
