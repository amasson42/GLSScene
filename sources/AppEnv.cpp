//
//  main.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "AppEnv.hpp"

#define USED_SCENE_CONTROLLER TrashSceneController

static void _keyCallBack(GLFWwindow *w, int k, int s, int a, int m) {
    AppEnv *e = static_cast<AppEnv*>(glfwGetWindowUserPointer(w));
    if (e == NULL)
        std::cout << "No input handle" << std::endl;
    else
        e->controller->keyCallBack(w, k, s, a, m);
}

AppEnv::AppEnv(const std::vector<std::string>& as) :
    args(as)
{
    if (!glfwInit()) {
        std::cerr << "Can't init GLFW" << std::endl;
        throw std::exception();
    }

    glfwWindowHint(GLFW_SAMPLES, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);

    windowWidth = 1200;
    windowHeight = 800;
    window = glfwCreateWindow(windowWidth, windowHeight, "openGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::exception();
    }
    glfwMakeContextCurrent(window);
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_MULTISAMPLE);

    try {
        GLS::glsInit();
    } catch (GLS::ShaderBuildingException& e) {
        std::cout << e.what() << std::endl;
        std::cout << e.infoLog << std::endl;
        glfwSetWindowShouldClose(window, true);
        glfwTerminate();
        throw std::exception();
    }

    glfwGetFramebufferSize(window, &windowBufferWidth, &windowBufferHeight);

    scene = std::make_shared<GLS::Scene>(glm::vec2(windowBufferWidth, windowBufferHeight));

    controller = new USED_SCENE_CONTROLLER(this);
    controller->makeScene();

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, _keyCallBack);

    scene->rootNode()->sendToFlux(std::cout, ":");

    currentTime = glfwGetTime();
    deltaTime = 0;
    fpsDisplayCD = 0;
}

AppEnv::~AppEnv() {
    std::cout << "Ending..." << std::endl;
    delete controller;
    scene = nullptr;
    GLS::glsDeinit();
    glfwTerminate();
}

std::shared_ptr<std::string> AppEnv::getArgument(std::string key) const {
    if (args.size() == 0)
        return nullptr;
    for (size_t i = 0; i < args.size() - 1; i++)
        if (args[i] == key)
            return std::make_shared<std::string>(args[i + 1]);
    return nullptr;
}

std::shared_ptr<GLS::Framebuffer> AppEnv::createEffectFramebuffer() {
    std::shared_ptr<GLS::Framebuffer> effectFrame = std::make_shared<GLS::Framebuffer>(windowBufferWidth, windowBufferHeight);
    std::shared_ptr<std::string> effectFilename = getArgument("-effect");
    if (effectFilename != nullptr) {
        std::ifstream effectShaderFile(*effectFilename);
        if (effectShaderFile.is_open()) {
            try {
                std::shared_ptr<GLS::Shader> effectShader = std::make_shared<GLS::Shader>(effectShaderFile, GL_FRAGMENT_SHADER);
                postProcessShaderProgram = std::make_shared<GLS::ShaderProgram>(*GLS::Shader::standardVertexScreenTexture(), *effectShader);
                effectFrame->setProgram(postProcessShaderProgram);
            } catch (GLS::ShaderBuildingException& e) {
                std::cerr << "Post processing effect compilation error" << std::endl;
                std::cerr << e.what() << std::endl;
                std::cerr << e.infoLog << std::endl;
            } catch (std::exception& e) {
                std::cerr << "Can't create post processing effect" << std::endl << e.what() << std::endl;
            }
        } else {
            std::cerr << "Can't open shader effect " << effectFilename << std::endl;
        }
    }
    return effectFrame;
}

void AppEnv::loop() {

    std::shared_ptr<GLS::Framebuffer> effectFramebuffer = createEffectFramebuffer();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        updateEventsState();

        scene->updateAnimations(deltaTime);
        controller->update();

        scene->renderInContext(effectFramebuffer);
        effectFramebuffer->unbind();
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        GLS::RenderUniforms uniforms;
        if (!scene->cameraNode().expired()) {
            std::shared_ptr<GLS::Node> cameraNode = scene->cameraNode().lock();
            glm::mat4 cameraMat = cameraNode->getWorldTransformMatrix();
            uniforms.camera_position = glm::vec3(cameraMat * glm::vec4(0, 0, 0, 1));
            uniforms.view = glm::inverse(cameraMat);
            if (cameraNode->camera() != nullptr) {
                uniforms.projection = cameraNode->camera()->projectionMatrix();
            }
        }
        effectFramebuffer->renderInContext(*scene, uniforms);

        if (displayFps()) {
            checkSize(effectFramebuffer);
        }

        glfwSwapBuffers(window);
    }

}

void AppEnv::updateEventsState() {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mousePosition = glm::vec2(mouseX, mouseY);
    if (postProcessShaderProgram != nullptr) {
        glm::vec2 devicePos = mouseContextPosition();
        postProcessShaderProgram->use();
        glUniform2f(postProcessShaderProgram->getLocation("u_mouse_position"), devicePos.x, devicePos.y);
        glUniform1f(postProcessShaderProgram->getLocation("u_time"), currentTime);
    }
    // std::cout << "mouse position: " << mousePosition << " -> " << mouseContextPosition() << std::endl;

    double newTime = glfwGetTime();
    deltaTime = newTime - currentTime;
    currentTime = newTime;
}

bool AppEnv::displayFps() {
    fpsDisplayCD -= deltaTime;
    if (fpsDisplayCD <= 0) {
        std::stringstream fpsss;
        fpsss << "FPS: " << 1.0 / deltaTime;
        glfwSetWindowTitle(window, fpsss.str().c_str());
        fpsDisplayCD = 0.5;
        return true;
    } else {
        return false;
    }
}

void AppEnv::checkSize(std::shared_ptr<GLS::Framebuffer> effectFramebuffer) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    if (width != windowWidth || height != windowHeight) {
        std::cout << "new size" << std::endl;
        windowWidth = width;
        windowHeight = height;
        glfwGetFramebufferSize(window, &windowBufferWidth, &windowBufferHeight);
        glm::vec2 newSize = glm::vec2(windowBufferWidth, windowBufferHeight);
        scene->setSize(newSize);
        effectFramebuffer = std::make_shared<GLS::Framebuffer>(windowBufferWidth, windowBufferHeight);
    }
}

glm::vec2 AppEnv::mouseContextPosition() const {
    return glm::vec2(1, -1) * mousePosition / glm::vec2(windowWidth / 2, windowHeight / 2) + glm::vec2(-1, 1);
}
