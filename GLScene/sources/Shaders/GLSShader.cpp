//
//  GLSShader.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSShader.hpp"

namespace GLS {

    void Shader::compile() {
        _shader = glCreateShader(_type);
        if (_shader == 0) {
            throw GLObjectCreationException(GLOBJECT_SHADER);
        }
        
        const char* rstr = _src.c_str();
        glShaderSource(_shader, 1, &rstr, NULL);
        glCompileShader(_shader);
        
        GLint success;
        glGetShaderiv(_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetShaderInfoLog(_shader, 1024, NULL, infoLog);
            glDeleteShader(_shader);
            throw ShaderBuildingException(SHADERBUILDING_COMPILATION, infoLog);
        }
    }
    
    Shader::Shader(std::ifstream& file, GLenum type) :
    _src(), _shader(0), _type(type) {
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
    
    Shader::Shader(std::string src, GLenum type) :
    _src(src), _shader(0), _type(type) {
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
            throw GLObjectCreationException(GLOBJECT_SHADERPROGRAM);
        }
        glAttachShader(_program, vertex._shader);
        glAttachShader(_program, fragment._shader);
        glLinkProgram(_program);
        
        GLint success;
        glGetProgramiv(_program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetProgramInfoLog(_program, 1024, NULL, infoLog);
            glDeleteProgram(_program);
            throw ShaderBuildingException(SHADERBUILDING_LINK, infoLog);
        }
    }

    ShaderProgram::ShaderProgram(const Shader& vertex, const Shader& geometry, const Shader& fragment) {
        _program = glCreateProgram();
        if (_program == 0) {
            throw GLObjectCreationException(GLOBJECT_SHADERPROGRAM);
        }
        glAttachShader(_program, vertex._shader);
        glAttachShader(_program, geometry._shader);
        glAttachShader(_program, fragment._shader);
        glLinkProgram(_program);

        GLint success;
        glGetProgramiv(_program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetProgramInfoLog(_program, 1024, NULL, infoLog);
            glDeleteProgram(_program);
            throw ShaderBuildingException(SHADERBUILDING_LINK, infoLog);
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
