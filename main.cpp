//
//  main.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include <iostream>

#ifdef __APPLE__
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include "GLScene.hpp"
#include <OpenGL/gl3.h>
#include <vector>

#include <GLFW/glfw3.h>
#include <cmath>

std::ostream& operator<<(std::ostream& stream, glm::vec3 v) {
    stream << '(' << v.x << ',' << v.y << ',' << v.z << ')';
    return stream;
}

std::ostream& operator<<(std::ostream& stream, glm::mat4 m) {
    const float *pSource = (const float*)glm::value_ptr(m);
    for (int i = 0; i < 16; i++) {
        stream << pSource[i] << ((i + 1) % 4 ? ' ' : '\n');
    }
    return stream;
}

void processInput(GLFWwindow *window, float deltaTime, GLS::Scene& scene);
int launch(std::vector<std::string>& modelNames);

int main(int argc, const char * argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
        args.push_back(argv[i]);
    launch(args);
    return 0;
}

void printNodePosition(const GLS::Node& node) {
    std::cout << "node position: " << node.transform().position() << std::endl;
    std::cout << "node transform: " << std::endl << node.getTransformMatrix() << std::endl;
}

int launch(std::vector<std::string>& modelNames) {
    
	static_cast<void>(modelNames);
	
    if (!glfwInit()) // init the lib once
        return (EXIT_FAILURE);
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    
    GLFWwindow *window = nullptr; // create a window pointer
    
    window = glfwCreateWindow(1200, 800, "openGL", nullptr, nullptr); // create the window
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window); // make context to draw to
    
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    
    /* create mesh */
    
    // try compilation of shaders
    try {
        GLS::ShaderProgram::standardShaderProgram();
    } catch (GLS::Shader::CompilationException& e) {
        std::cout << e.what() << std::endl;
        std::cout << e.infoLog() << std::endl;
    }

    // create simple mesh

    GLS::Scene scene;
    
    std::shared_ptr<GLS::Node> triangleNode = std::make_shared<GLS::Node>();
    {
        std::shared_ptr<GLS::Mesh> triangleMesh = std::make_shared<GLS::Mesh>();
        triangleMesh->verticesRef().push_back(GLS::Vertex(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec4(1), glm::vec2(0, 0)));
        triangleMesh->verticesRef().push_back(GLS::Vertex(glm::vec3(1, 0, 0), glm::vec3(0, 0, -1), glm::vec4(1), glm::vec2(1, 0)));
        triangleMesh->verticesRef().push_back(GLS::Vertex(glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec4(1), glm::vec2(0, 1)));
        triangleMesh->indicesRef().push_back(0);
        triangleMesh->indicesRef().push_back(1);
        triangleMesh->indicesRef().push_back(2);
        triangleMesh->generateBuffers();
        triangleNode->addRenderable(triangleMesh);
    }
    
    scene.rootNode().addChildNode(triangleNode);

    std::shared_ptr<GLS::Node> cameraNode = std::make_shared<GLS::Node>();
    {
        std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
        cameraNode->setCamera(camera);
    }
    scene.setCameraNode(*cameraNode);
    scene.backgroundColor = glm::vec4(0.2, 0.5, 0.2, 1.0f);

    //
    
    float lastTimeUpdate = glfwGetTime();
    while (!glfwWindowShouldClose(window)) { // loop while not closed
        glfwPollEvents(); // check a loop turn

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTimeUpdate;
        if (true)
            processInput(window, deltaTime, scene);

        scene.renderInContext();
        /* do some drawing */
        
        glfwSwapBuffers(window); // draw the new image to the buffer
        lastTimeUpdate = currentTime;
    }
    
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
            cam.transform().setPosition(cam.transform().position() + cameraSpeed * cameraFront);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cam.transform().setPosition(cam.transform().position() - cameraSpeed * cameraFront);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cam.transform().setPosition(cam.transform().position() - cameraSpeed * cameraRight);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cam.transform().setPosition(cam.transform().position() + cameraSpeed * cameraRight);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            cam.transform().setPosition(cam.transform().position() + cameraSpeed * cameraUp);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            cam.transform().setPosition(cam.transform().position() - cameraSpeed * cameraUp);
    }
}
