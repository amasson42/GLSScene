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
    
    /* create mesh */
    
    // try compilation of shaders
    try {
        GLS::ShaderProgram::standardShaderProgramMesh();
        GLS::ShaderProgram::standardShaderProgramMeshOutline();
        GLS::ShaderProgram::standardShaderProgramScreenTexture();
    } catch (GLS::Shader::CompilationException& e) {
        std::cout << e.what() << std::endl;
        std::cout << e.infoLog() << std::endl;
        glfwSetWindowShouldClose(window, true);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    std::cout << "shaders compiled" << std::endl;
    // create simple mesh

    GLS::Scene scene;
    
    std::shared_ptr<GLS::Node> triangleNode = std::make_shared<GLS::Node>();
    {
        std::shared_ptr<GLS::Mesh> triangleMesh = std::make_shared<GLS::Mesh>();
        triangleMesh->verticesRef().push_back(GLS::Vertex(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec2(0, 0)));
        triangleMesh->verticesRef().push_back(GLS::Vertex(glm::vec3(1, 0, 0), glm::vec3(0, 0, -1), glm::vec2(1, 0)));
        triangleMesh->verticesRef().push_back(GLS::Vertex(glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec2(0, 1)));
        triangleMesh->indicesRef().push_back(0);
        triangleMesh->indicesRef().push_back(1);
        triangleMesh->indicesRef().push_back(2);
        triangleMesh->generateBuffers();
        triangleNode->addRenderable(triangleMesh);
        triangleMesh->getMaterial()->diffuse = glm::vec3(1.0, 0.0, 0.0);
    }
    scene.rootNode().addChildNode(triangleNode);

    std::shared_ptr<GLS::Node> planeNode = std::make_shared<GLS::Node>();
    planeNode->addRenderable(GLS::Mesh::plane(1.0, 1.0));
    scene.rootNode().addChildNode(planeNode);

    std::shared_ptr<GLS::Node> sphereNode = std::make_shared<GLS::Node>();
    {
        std::shared_ptr<GLS::Mesh> sphereMesh = GLS::Mesh::sphere(1.0);
        sphereMesh->getMaterial()->diffuse = glm::vec3(0.5, 0.1, 0.2);
        sphereMesh->generateBuffers();
        sphereNode->addRenderable(sphereMesh);
    }
    planeNode->addChildNode(sphereNode);
    sphereNode->transform().setPosition(glm::vec3(2, 1, 0));

    std::shared_ptr<GLS::Node> nsNode = std::make_shared<GLS::Node>();
    nsNode->loadMeshFromFile("../models/nanosuit/nanosuit.obj");
    scene.rootNode().addChildNode(nsNode);
    std::cout << "the nanosuit node has " << nsNode->renderables().size() << " meshes" << std::endl;

    std::shared_ptr<GLS::Node> ftNode = std::make_shared<GLS::Node>();
    ftNode->loadMeshFromFile("../models/42.obj");
    scene.rootNode().addChildNode(ftNode);

    std::shared_ptr<GLS::Node> spotlightNode = std::make_shared<GLS::Node>();
    std::shared_ptr<GLS::Light> spotlight = std::make_shared<GLS::Light>();
    spotlight->setType(GLS::light_spot);
    spotlight->setPosition(glm::vec3(0, 4, 0));
    spotlight->setDirection(glm::vec3(4, -1, 0));
    spotlight->setAngle(1.5);
    spotlightNode->setLight(spotlight);
    scene.rootNode().addChildNode(spotlightNode);

    std::shared_ptr<GLS::Node> pointLightNode = std::make_shared<GLS::Node>();
    std::shared_ptr<GLS::Light> pointlight = std::make_shared<GLS::Light>();
    pointlight->setType(GLS::light_point);
    pointlight->setPosition(glm::vec3(0, 15, -7));
    pointLightNode->setLight(pointlight);
    scene.rootNode().addChildNode(pointLightNode);

    std::shared_ptr<GLS::Node> cubeNode = std::make_shared<GLS::Node>();
    std::shared_ptr<GLS::Mesh> cubeMesh = GLS::Mesh::cube(1.5, 1.5, 1.5);
    try {
        std::shared_ptr<GLS::Texture> diffuse(new GLS::Texture("../textures/pavement/albedo.png", GL_RGB));
        std::shared_ptr<GLS::Texture> normal(new GLS::Texture("../textures/pavement/normal.png", GL_RGB));
        std::shared_ptr<GLS::Texture> occlusion(new GLS::Texture("../textures/pavement/occlusion.png", GL_RGB));
        std::shared_ptr<GLS::Texture> roughness(new GLS::Texture("../textures/pavement/roughness.png", GL_RGB));
        cubeMesh->getMaterial()->texture_diffuse = diffuse;
        cubeMesh->getMaterial()->texture_normal = normal;
        cubeMesh->getMaterial()->texture_occlusion = occlusion;
        cubeMesh->getMaterial()->texture_roughness = roughness;
        cubeMesh->setOutline(0.1, glm::vec3(1));
    } catch (std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
    cubeNode->setName("cube");
    cubeNode->addRenderable(cubeMesh);
    cubeNode->transform().setPosition(glm::vec3(4, 1, 0));
    scene.rootNode().addChildNode(cubeNode);

    std::shared_ptr<GLS::Node> cameraNode = std::make_shared<GLS::Node>();
    {
        std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
        camera->setAspect(win_width / (float)win_height);
        camera->setFarZ(15.0);
        cameraNode->setCamera(camera);
    }
    cameraNode->transform().moveBy(0, 7, 5);
    scene.setCameraNode(*cameraNode);

    //

    GLS::Framebuffer facebook(win_width, win_height);
    {
        GLS::Framebuffer fb(500, 250);
    }

    float lastTimeUpdate = glfwGetTime();
    while (!glfwWindowShouldClose(window)) { // loop while not closed
        glfwPollEvents(); // check a loop turn

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTimeUpdate;
        if (true)
            processInput(window, deltaTime, scene);

        planeNode->transform().setRotation(glm::angleAxis(currentTime, glm::vec3(0, 1, 0)));
        cubeMesh->getMaterial()->diffuse_transform.rotateBy(0.02);
        pointLightNode->transform().setRotation(glm::angleAxis(currentTime, glm::vec3(0, 1, 0)));

        /* do some drawing */
        facebook.bind();
        scene.renderInContext();
        facebook.unbind();
        facebook.renderInContext();

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
        
    }
}
