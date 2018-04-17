//
//  main.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include <iostream>
#include "GLScene.hpp"
#include <GLFW/glfw3.h>

std::ostream& operator<<(std::ostream& out, const GLS::Matrix4x4& m) {
    out << m(0, 0) << ' ' << m(0, 1) << ' ' << m(0, 2) << ' ' << m(0, 3) << '\n';
    out << m(1, 0) << ' ' << m(1, 1) << ' ' << m(1, 2) << ' ' << m(1, 3) << '\n';
    out << m(2, 0) << ' ' << m(2, 1) << ' ' << m(2, 2) << ' ' << m(2, 3) << '\n';
    out << m(3, 0) << ' ' << m(3, 1) << ' ' << m(3, 2) << ' ' << m(3, 3) << '\n';
    return out;
}

int launch(const char *name);

int main(int argc, const char * argv[]) {
    launch("coucou");
    return 0;
}

int launch(const char *name) {
    
    if (!glfwInit()) // init the lib once
        return (EXIT_FAILURE);
    
    GLFWwindow *window = nullptr; // create a window pointer
    
    window = glfwCreateWindow(1200, 800, name, nullptr, nullptr); // create the window
    
    std::ifstream fragmentFile;
    fragmentFile.open("/Users/arthur/Documents/testProg/C/openGL/glscene/shaders/fragment.glsl");
    GLS::Shader fragmentShader(fragmentFile, GL_FRAGMENT_SHADER);
    std::ifstream vertexFile;
    vertexFile.open("/Users/arthur/Documents/testProg/C/openGL/glscene/shaders/vertex.glsl");
    GLS::Shader vertexShader(vertexFile, GL_VERTEX_SHADER);
    GLS::ShaderProgram shdprgm(vertexShader, fragmentShader);
    
    glfwMakeContextCurrent(window); // make context to draw to
    
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    GLS::Scene scene;
    
    while (!glfwWindowShouldClose(window)) { // loop while not closed
        
        scene.renderInContextWithShader(shdprgm);
        /* do some drawing */
        
        glfwSwapBuffers(window); // draw the new image to the buffer
        glfwPollEvents(); // check a loop turn
    }
    
    glfwTerminate(); // free all used memory of glfw
    
    return (EXIT_SUCCESS);
}
