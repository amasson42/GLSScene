//
//  main.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "sceneTest.hpp"

// void (*loadScene)(const AppEnv& env)     = loadSceneTrash;
// void (*updateScene)(const AppEnv& env)   = nullptr;

// void (*loadScene)(const AppEnv& env)     = loadSceneHuman;
// void (*updateScene)(const AppEnv& env)   = updateSceneHuman;

void (*loadScene)(const AppEnv& env)     = loadSceneShadow;
void (*updateScene)(const AppEnv& env)   = updateSceneShadow;

// void (*loadScene)(const AppEnv& env)     = loadSceneVoxel;
// void (*updateScene)(const AppEnv& env)   = updateSceneVoxel;

// void (*loadScene)(const AppEnv& env)     = loadSceneVoxelProcedural;
// void (*updateScene)(const AppEnv& env)   = updateSceneVoxelProcedural;

// void (*loadScene)(const AppEnv& env)     = loadSceneParticuleSystem;
// void (*updateScene)(const AppEnv& env)   = updateSceneParticuleSystem;

AppEnv::AppEnv(const std::vector<std::string>& as) :
    args(as),
    mustUpdate(true)
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
    } catch (GLS::Shader::CompilationException& e) {
        std::cout << e.what() << std::endl;
        std::cout << e.infoLog() << std::endl;
        glfwSetWindowShouldClose(window, true);
        glfwTerminate();
        throw std::exception();
    }

    glfwGetFramebufferSize(window, &windowBufferWidth, &windowBufferHeight);

    scene = std::make_shared<GLS::Scene>(glm::vec2(windowBufferWidth, windowBufferHeight));
    loadScene(*this);

    currentTime = glfwGetTime();
    deltaTime = 0;
    fpsDisplayCD = 0;
}

AppEnv::~AppEnv() {
    std::cout << "Ending..." << std::endl;
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
                std::shared_ptr<GLS::ShaderProgram> effectProgram = std::make_shared<GLS::ShaderProgram>(*GLS::Shader::standardVertexScreenTexture(), *effectShader);
                effectFrame->setProgram(effectProgram);
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

        double newTime = glfwGetTime();
        deltaTime = newTime - currentTime;
        currentTime = newTime;

        processInput();

        if (mustUpdate && updateScene != nullptr)
            updateScene(*this);

        scene->renderInContext(effectFramebuffer);
        effectFramebuffer->unbind();
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        effectFramebuffer->renderInContext(*scene, GLS::RenderUniforms());

        if (displayFps()) {
            checkSize(effectFramebuffer);
        }

        glfwSwapBuffers(window);
    }

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
    glfwGetFramebufferSize(window, &windowBufferWidth, &windowBufferHeight);
    glm::vec2 newSize = glm::vec2(windowBufferWidth, windowBufferHeight);
    if (newSize != scene->getSize()) {
        scene->setSize(newSize);
        effectFramebuffer = std::make_shared<GLS::Framebuffer>(windowBufferWidth, windowBufferHeight);
    }
}

void AppEnv::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (!scene->cameraNode().expired()) {
        GLS::Node& cam(*scene->cameraNode().lock());
        static float cameraAngleX = cam.transform().eulerAngles().x;
        static float cameraAngleY = cam.transform().eulerAngles().y;

        float cameraSpeed = 5.0 * deltaTime;
        glm::mat4 cameraMat = cam.getWorldTransformMatrix();
        glm::vec3 cameraFront = glm::vec3(cameraMat * glm::vec4(0, 0, -1, 0));
        glm::vec3 cameraRight = glm::vec3(cameraMat * glm::vec4(1, 0, 0, 0));
        glm::vec3 cameraUp = glm::vec3(cameraMat * glm::vec4(0, 1, 0, 0));

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cam.transform().moveBy(cameraSpeed * cameraFront);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cam.transform().moveBy(-cameraSpeed * cameraFront);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cam.transform().moveBy(-cameraSpeed * cameraRight);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cam.transform().moveBy(cameraSpeed * cameraRight);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            cam.transform().moveBy(cameraSpeed * cameraUp);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            cam.transform().moveBy(-cameraSpeed * cameraUp);

        float cameraRotateSpeed = 3.0 * deltaTime;
        bool changeCamera = true;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            cameraAngleY += cameraRotateSpeed;
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            cameraAngleY -= cameraRotateSpeed;
        else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            cameraAngleX -= cameraRotateSpeed;
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            cameraAngleX += cameraRotateSpeed;
        else
            changeCamera = false;

        if (changeCamera)
            cam.transform().setEulerAngles(cameraAngleX, cameraAngleY, 0);
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        if (glIsEnabled(GL_FRAMEBUFFER_SRGB))
            glDisable(GL_FRAMEBUFFER_SRGB);
        else
            glEnable(GL_FRAMEBUFFER_SRGB);
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        mustUpdate = false;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        mustUpdate = true;
    }

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mousePosition = glm::vec2(mouseX, mouseY);
    // std::cout << "mouse position: " << mousePosition << " -> " << mouseContextPosition() << std::endl;
    // windowMousePos = glm::vec2(1, -1) * glm::vec2(mouseX, mouseY) / glm::vec2(win_buffer_width / 2, win_buffer_height / 2) + glm::vec2(-1, 1);
}

glm::vec2 AppEnv::mouseContextPosition() const {
    return glm::vec2(2, -2) * mousePosition / glm::vec2(windowBufferWidth / 2, windowBufferHeight / 2) + glm::vec2(-1, 1);
}
