
#pragma once

#ifdef __APPLE__
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include "GLScene.hpp"

#include <GLFW/glfw3.h>

#include "AppEnv.hpp"

class GLSWindow {

    AppEnv *_env;
    GLFWwindow *_glfwWindow;
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

    GLSWindow(AppEnv *env, glm::vec2 size, std::string title);
    virtual ~GLSWindow();

    void setTitle(std::string title);
    void setPostProcessEffect(std::shared_ptr<GLS::ShaderProgram> program);
    void setController(std::shared_ptr<ISceneController> controller);

    // Getters
    bool isActive() const;
    std::shared_ptr<GLS::Scene> scene();
    std::shared_ptr<ISceneController> controller();
    AppEnv *getAppEnvPtr() const;

    // Window events
    void close();
    void updateSize();
    void updateEvents();
    void keyCallBack(int key, int scan, int action, int mods);

    glm::vec2 mousePosition() const;
    glm::vec2 mouseContextPosition() const;

    bool keyPressed(int key) const;

    float elapsedTime() const;
    float deltaTime() const;

    // GameLoop
    void loopOnce();
};