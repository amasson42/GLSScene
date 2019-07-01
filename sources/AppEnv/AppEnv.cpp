//
//  main.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "AppEnv.hpp"


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

    std::shared_ptr<GLSWindow> mainWindow;
    try {
        mainWindow = std::make_shared<GLSWindow>(this, glm::vec2(1200, 800), "GLS");
    } catch (std::exception& e) {
        std::cerr << "Error during main window creation" << std::endl;
        glfwTerminate();
        throw e;
    }
    windows.push_back(mainWindow);

    // TODO: do the controller choice
    sceneController = std::make_shared<TrashSceneController>(mainWindow);

    mainWindow->setController(sceneController);

    std::shared_ptr<std::string> effectFilename = getArgument("-effect");
    if (effectFilename != nullptr) {
        std::ifstream effectShaderFile(*effectFilename);
        if (effectShaderFile.is_open()) {
            try {
                std::shared_ptr<GLS::Shader> effectShader = std::make_shared<GLS::Shader>(effectShaderFile, GL_FRAGMENT_SHADER);
                std::shared_ptr<GLS::ShaderProgram> program = std::make_shared<GLS::ShaderProgram>(*GLS::Shader::standardVertexScreenTexture(), *effectShader);
                mainWindow->setPostProcessEffect(program);
            } catch (GLS::ShaderBuildingException& e) {
                std::cerr << "Post processing effect compilation error" << std::endl;
                std::cerr << e.what() << std::endl;
                std::cerr << e.infoLog << std::endl;
            } catch (std::exception& e) {
                std::cerr << "Exception during post processing program creation :" << std::endl;
                std::cerr << e.what() << std::endl;
            }
        } else {
            std::cerr << "Can't open shader effect " << *effectFilename << std::endl;
        }
    }

    sceneController->makeScene();

}

AppEnv::~AppEnv() {
    std::cout << "Ending..." << std::endl;
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

void AppEnv::loop() {

    while (!windows.empty()) {
        glfwPollEvents();
        std::vector<std::shared_ptr<GLSWindow> >::iterator it = windows.begin();
        while (it != windows.end()) {
            if ((*it)->isActive()) {
                (*it++)->loopOnce();
            } else {
                it = windows.erase(it);
            }
        }
    }

}
