//
//  main.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include <iostream>
#include <stdio.h>

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

int main(int argc, const char * argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
        args.push_back(argv[i]);
    launch(args);
    if (!args.empty() && args[0] == "-leaks") {
        std::cout << "test leaks..." << std::endl;
        while (1);
    }
    return 0;
}

void printNodePosition(const GLS::Node& node) {
    std::cout << "node position: " << node.transform().position() << std::endl;
    std::cout << "node transform: " << std::endl << node.getTransformMatrix() << std::endl;
}

void loadScene1(GLS::Scene& scene);
void loadScene2(GLS::Scene& scene);
void updateScene2(double et, double dt);
void loadScene3(GLS::Scene& scene);
void updateScene3(double et, double dt);

auto loadScene = loadScene3;
auto updateScene = updateScene3;

int launch(std::vector<std::string>& modelNames) {

	static_cast<void>(modelNames);

    if (!glfwInit()) // init the lib once
        return (EXIT_FAILURE);

    glfwWindowHint(GLFW_SAMPLES, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    GLFWwindow *window = nullptr; // create a window pointer

    const int win_width = 1200, win_height = 800, win_margin = 0;
    window = glfwCreateWindow(win_width, win_height, "openGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window); // make context to draw to
    glViewport(win_margin, win_margin,
            win_width - 2 * win_margin, win_height - 2 * win_margin);

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_MULTISAMPLE);

    // try compilation of shaders
    try {
        GLS::ShaderProgram::standardShaderProgramMesh();
        GLS::ShaderProgram::standardShaderProgramMeshSimpleColor();
        GLS::ShaderProgram::standardShaderProgramScreenTexture();
        GLS::ShaderProgram::standardShaderProgramSkybox();
    } catch (GLS::Shader::CompilationException& e) {
        std::cout << e.what() << std::endl;
        std::cout << e.infoLog() << std::endl;
        glfwSetWindowShouldClose(window, true);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    std::cout << "shaders compiled" << std::endl;
    // create simple mesh

    GLS::Scene scene(glm::vec2(2 * win_width, 2 * win_height));
    loadScene(scene);

    //

    float lastTimeUpdate = glfwGetTime();
    while (!glfwWindowShouldClose(window)) { // loop while not closed
        glfwPollEvents(); // check a loop turn

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTimeUpdate;
        if (true)
            processInput(window, deltaTime, scene);

        updateScene(currentTime, deltaTime);

        /* do some drawing */
        scene.renderInContext();
        // facebook.bind();
        // scene.renderInContext();
        // facebook.unbind();
        // facebook.renderInContext();

        glfwSwapBuffers(window); // draw the new image to the buffer
        lastTimeUpdate = currentTime;
    }

    std::cout << "Ending..." << std::endl;
    glfwTerminate(); // free all used memory of glfw
    return (EXIT_SUCCESS);
}

void processInput(GLFWwindow *window, float deltaTime, GLS::Scene& scene) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (scene.cameraNode()) {
        GLS::Node& cam(*scene.cameraNode());

        float cameraSpeed = 2.5 * deltaTime;
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

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            cam.transform().rotateEulerAnglesBy(0, cameraSpeed, 0);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            cam.transform().rotateEulerAnglesBy(0, -cameraSpeed, 0);
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            cam.transform().rotateEulerAnglesBy(-cameraSpeed, 0, 0);
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            cam.transform().rotateEulerAnglesBy(cameraSpeed, 0, 0);

        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            if (glIsEnabled(GL_FRAMEBUFFER_SRGB))
                glDisable(GL_FRAMEBUFFER_SRGB);
            else
                glEnable(GL_FRAMEBUFFER_SRGB);
        }

    }
}
