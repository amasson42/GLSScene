//
//  main.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include <iostream>
#include <sstream>

#ifdef __APPLE__
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include "GLScene.hpp"
#include <vector>

#include <GLFW/glfw3.h>
#include <cmath>
#include <stdlib.h>

void processInput(GLFWwindow *window, float deltaTime, GLS::Scene& scene);
int launch(std::vector<std::string>& modelNames);

void prelaunch(int argc, char const *argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-leaks") != 0)
            args.push_back(argv[i]);
    }
    launch(args);
}

int main(int argc, const char * argv[]) {
    prelaunch(argc, argv);
    if (argc >= 2 && strcmp(argv[1], "-leaks") == 0) {
        std::cout << "test leaks..." << std::endl;
        while (1)
            sleep(1);
    }
    return 0;
}

void printNodePosition(const GLS::Node& node) {
    std::cout << "node position: " << node.transform().position() << std::endl;
    std::cout << "node transform: " << std::endl << node.getTransformMatrix() << std::endl;
}

void loadScene1(GLS::Scene& scene, const std::vector<std::string>& args);
void loadScene2(GLS::Scene& scene, const std::vector<std::string>& args);
void loadScene3(GLS::Scene& scene, const std::vector<std::string>& args);
void loadSceneVoxel(GLS::Scene& scene, const std::vector<std::string>& args);
void loadSceneVoxelProcedural(GLS::Scene& scene, const std::vector<std::string>& args);
void loadSceneParticuleSystem(GLS::Scene& scene, const std::vector<std::string>& args);

void updateScene2(double et, double dt);
void updateScene3(double et, double dt);
void updateSceneVoxel(double et, double dt);
void updateSceneVoxelProcedural(double et, double dt);
void updateSceneParticuleSystem(double et, double dt);

// void (*loadScene)(GLS::Scene&, const std::vector<std::string>&)     = loadScene1;
// void (*updateScene)(double, double)                                 = nullptr;

// void (*loadScene)(GLS::Scene&, const std::vector<std::string>&)     = loadScene2;
// void (*updateScene)(double, double)                                 = updateScene2;

// void (*loadScene)(GLS::Scene&, const std::vector<std::string>&)     = loadScene3;
// void (*updateScene)(double, double)                                 = updateScene3;

// void (*loadScene)(GLS::Scene&, const std::vector<std::string>&)     = loadSceneVoxel;
// void (*updateScene)(double, double)                                 = updateSceneVoxel;

void (*loadScene)(GLS::Scene&, const std::vector<std::string>&)     = loadSceneVoxelProcedural;
void (*updateScene)(double, double)                                 = updateSceneVoxelProcedural;

// void (*loadScene)(GLS::Scene&, const std::vector<std::string>&)     = loadSceneParticuleSystem;
// void (*updateScene)(double, double)                                 = updateSceneParticuleSystem;

bool mustUpdate = true;

GLFWwindow *window = nullptr;
glm::vec2 windowMousePos = glm::vec2(0);
int win_buffer_width, win_buffer_height;

int launch(std::vector<std::string>& args) {
    (void)args;
    if (!glfwInit()) // init the lib once
        return (EXIT_FAILURE);

    glfwWindowHint(GLFW_SAMPLES, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    window = nullptr; // create a window pointer

    const int window_width = 1200, window_height = 800;
    window = glfwCreateWindow(window_width, window_height, "openGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window); // make context to draw to

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_MULTISAMPLE);

    // try compilation of shaders
    try {
        GLS::glsInit();
    } catch (GLS::Shader::CompilationException& e) {
        std::cout << e.what() << std::endl;
        std::cout << e.infoLog() << std::endl;
        glfwSetWindowShouldClose(window, true);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    std::cout << "shaders compiled" << std::endl;
    // create simple mesh

    glfwGetFramebufferSize(window, &win_buffer_width, &win_buffer_height);
    std::shared_ptr<GLS::Scene> scene = std::make_shared<GLS::Scene>(glm::vec2(win_buffer_width, win_buffer_height));
    loadScene(*scene, args);

    //

    std::shared_ptr<GLS::Framebuffer> facebook = std::make_shared<GLS::Framebuffer>(win_buffer_width, win_buffer_height);

    float lastTimeUpdate = glfwGetTime();
    float fpsDisplayCD = 0.5;

    while (!glfwWindowShouldClose(window)) { // loop while not closed
        glfwPollEvents(); // check a loop turn

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTimeUpdate;
        fpsDisplayCD -= deltaTime;
        if (true)
            processInput(window, deltaTime, *scene);
        
        if (mustUpdate && updateScene != nullptr)
            updateScene(currentTime, deltaTime);

        /* do some drawing */

        if (1) {
            scene->renderInContext(facebook);
            facebook->unbind();
            glClearColor(1, 1, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            facebook->renderInContext(*scene, GLS::RenderUniforms());
        } else {
            scene->renderInContext();
        }

        if (fpsDisplayCD <= 0) {
            std::stringstream fpsss;
            fpsss << "FPS: " << 1.0 / deltaTime;
            glfwSetWindowTitle(window, fpsss.str().c_str());
            fpsDisplayCD = 0.5;
            glfwGetFramebufferSize(window, &win_buffer_width, &win_buffer_height);
            scene->setSize(glm::vec2(win_buffer_width, win_buffer_height));
            facebook = std::make_shared<GLS::Framebuffer>(win_buffer_width, win_buffer_height);
        }

        glfwSwapBuffers(window); // draw the new image to the buffer
        lastTimeUpdate = currentTime;
    }

    std::cout << "Ending..." << std::endl;
    scene = nullptr;
    GLS::glsDeinit();
    glfwTerminate(); // free all used memory of glfw
    return (EXIT_SUCCESS);
}

void processInput(GLFWwindow *window, float deltaTime, GLS::Scene& scene) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (!scene.cameraNode().expired()) {
        GLS::Node& cam(*scene.cameraNode().lock());
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
    windowMousePos = glm::vec2(1, -1) * glm::vec2(mouseX, mouseY) / glm::vec2(win_buffer_width / 2, win_buffer_height / 2) + glm::vec2(-1, 1);
}
