//
//  GLSShader.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSShader.hpp"
#include <iostream>

namespace GLS {
    
    void Shader::compile() {
        _shader = glCreateShader(_type);
        if (_shader == 0) {
            // TODO: throw shader creation error
            return ;
        }
        
        const char* rstr = _src.c_str();
        glShaderSource(_shader, 1, &rstr, NULL);
        glCompileShader(_shader);
        
        GLint success;
        GLchar infolog[1024];
        glGetShaderiv(_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(_shader, 1024, NULL, infolog);
            std::cerr << "compile shader error :" << infolog << std::endl;
            // TODO: throw compile error
        } else {
            std::cout << "shader compile success" << std::endl;
        }
    }
    
    Shader::Shader(std::ifstream& file, GLenum type) : _src(), _shader(0), _type(type) {
        file.seekg(0, file.end);
        size_t length = file.tellg();
        file.seekg(0, file.beg);
        char *buffer = new char[length + 1];
        file.read(buffer, length);
        buffer[length] = '\0';
        _src = buffer;
        delete[] buffer;
        compile();
    }
    
    Shader::Shader(std::string src, GLenum type) : _src(src), _shader(0), _type(type) {
        compile();
    }
    
    Shader::~Shader() {
        if (_shader)
            glDeleteShader(_shader);
    }
    
    GLenum Shader::type() const {
        return _type;
    }
    
    void Shader::clearSrcs() {
        _src.clear();
    }
    
    ShaderProgram::ShaderProgram(const Shader& vertex, const Shader& fragment) {
        _program = glCreateProgram();
        if (_program == 0) {
            // TODO: throw program creation error
        }
        glAttachShader(_program, vertex._shader);
        glAttachShader(_program, fragment._shader);
        glLinkProgram(_program);
        
        GLint success;
        GLchar infolog[1024];
        glGetProgramiv(_program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(_program, 1024, NULL, infolog);
            std::cerr << "links program error: " << infolog << std::endl;
            glDeleteProgram(_program);
            // TODO: throw program link error
        }
    }
    
    ShaderProgram::~ShaderProgram() {
        glDeleteProgram(_program);
    }
    
    GLuint ShaderProgram::program() const {
        return _program;
    }
    
    GLint ShaderProgram::getLocation(std::string name) const {
        return glGetUniformLocation(_program, name.c_str());
    }
    
    void ShaderProgram::use() const {
        glUseProgram(_program);
    }
    
}
