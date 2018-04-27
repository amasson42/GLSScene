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

//#include "GLScene.hpp"
#include <OpenGL/gl3.h>
#include <vector>

#include <GLFW/glfw3.h>
#include <cmath>

/*
std::ostream& operator<<(std::ostream& out, const GLS::Matrix4x4& m) {
    out << m(0, 0) << ' ' << m(0, 1) << ' ' << m(0, 2) << ' ' << m(0, 3) << '\n';
    out << m(1, 0) << ' ' << m(1, 1) << ' ' << m(1, 2) << ' ' << m(1, 3) << '\n';
    out << m(2, 0) << ' ' << m(2, 1) << ' ' << m(2, 2) << ' ' << m(2, 3) << '\n';
    out << m(3, 0) << ' ' << m(3, 1) << ' ' << m(3, 2) << ' ' << m(3, 3) << '\n';
    return out;
}
*/

int launch(std::vector<std::string>& modelNames);

int main(int argc, const char * argv[]) {
    
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
        args.push_back(argv[i]);
    launch(args);
    return 0;
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
    
/*    std::ifstream vertexFile;
    vertexFile.open("/Users/arthur/Documents/testProg/C/openGL/glscene/shaders/vertex.glsl");
    GLS::Shader vertexShader(vertexFile, GL_VERTEX_SHADER);
    std::ifstream fragmentFile;
    fragmentFile.open("/Users/arthur/Documents/testProg/C/openGL/glscene/shaders/fragment.glsl");
    GLS::Shader fragmentShader(fragmentFile, GL_FRAGMENT_SHADER);
    GLS::ShaderProgram program(vertexShader, fragmentShader);
    
    GLS::Mesh triangle;
    triangle.verticesRef().push_back(GLS::Vertex(GLS::Vector(0, 0, 0), GLS::Vector(0, 0, -1), GLS::Color(), GLS::Vector(0, 0)));
    triangle.verticesRef().push_back(GLS::Vertex(GLS::Vector(1, 0, 0), GLS::Vector(0, 0, -1), GLS::Color(), GLS::Vector(1, 0)));
    triangle.verticesRef().push_back(GLS::Vertex(GLS::Vector(0, 1, 0), GLS::Vector(0, 0, -1), GLS::Color(), GLS::Vector(0, 1)));
    
    triangle.indicesRef().push_back(0);
    triangle.indicesRef().push_back(1);
    triangle.indicesRef().push_back(2);
    
    triangle.generateBuffers();
*/
	
    //
    
    while (!glfwWindowShouldClose(window)) { // loop while not closed
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        /* do some drawing */
        
        glfwSwapBuffers(window); // draw the new image to the buffer
        glfwPollEvents(); // check a loop turn
    }
    
    glfwTerminate(); // free all used memory of glfw
    
    return (EXIT_SUCCESS);
}
