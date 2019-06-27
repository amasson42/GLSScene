//
//  sceneTest.hpp
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

struct AppEnv {

    std::vector<std::string> args;
    GLFWwindow *window;

    int windowWidth;
    int windowHeight;

    int windowBufferWidth;
    int windowBufferHeight;

    glm::vec2 mousePosition;

    std::shared_ptr<GLS::Scene> scene;

    bool mustUpdate;
    double currentTime;
    double deltaTime;
    double fpsDisplayCD;

    std::shared_ptr<GLS::ShaderProgram> postProcessShaderProgram;

    AppEnv(const std::vector<std::string>& as);

    ~AppEnv();

    glm::vec2 mouseContextPosition() const;
    std::shared_ptr<std::string> getArgument(std::string key) const;

    std::shared_ptr<GLS::Framebuffer> createEffectFramebuffer();
    void loop();
    bool displayFps();
    void checkSize(std::shared_ptr<GLS::Framebuffer> effectFramebuffer);
    void processInput();
};

void loadSceneTrash(const AppEnv& env);
void loadSceneHuman(const AppEnv& env);
void loadSceneShadow(const AppEnv& env);
void loadSceneVoxel(const AppEnv& env);
void loadSceneVoxelProcedural(const AppEnv& env);
void loadSceneParticuleSystem(const AppEnv& env);

void updateSceneHuman(const AppEnv& env);
void updateSceneShadow(const AppEnv& env);
void updateSceneVoxel(const AppEnv& env);
void updateSceneVoxelProcedural(const AppEnv& env);
void updateSceneParticuleSystem(const AppEnv& env);

void initNoise(unsigned int seed);
double noise(double x, double y, double z);
