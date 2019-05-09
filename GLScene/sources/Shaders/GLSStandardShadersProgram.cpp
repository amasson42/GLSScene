//
//  GLSStandardShadersProgram.cpp
//  GLScene
//
//  Created by Arthur MASSON on 4/27/18.
//  Copyright © 2018 Arthur MASSON. All rights reserved.
//

#include "GLSShader.hpp"
#include <iostream>

namespace GLS {

    std::shared_ptr<ShaderProgram> ShaderProgram::_standardShaderProgramMesh = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::standardShaderProgramMesh() {
        if (_standardShaderProgramMesh == nullptr) {
            std::shared_ptr<Shader> vertex = Shader::standardVertexMesh();
            std::shared_ptr<Shader> fragment = Shader::standardFragmentMesh();
            _standardShaderProgramMesh = std::make_shared<ShaderProgram>(*vertex, *fragment);
        }
        return _standardShaderProgramMesh;
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::_standardShaderProgramMeshOutline = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::standardShaderProgramMeshOutline() {
        if (_standardShaderProgramMeshOutline == nullptr) {
            std::shared_ptr<Shader> vertex = Shader::standardVertexMesh();
            std::shared_ptr<Shader> fragment = Shader::standardFragmentMeshOutline();
            _standardShaderProgramMeshOutline = std::make_shared<ShaderProgram>(*vertex, *fragment);
        }
        return _standardShaderProgramMeshOutline;
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::_standardShaderProgramInstancedMesh = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::standardShaderProgramInstancedMesh() {
        if (_standardShaderProgramInstancedMesh == nullptr) {
            std::shared_ptr<Shader> vertex = Shader::standardVertexInstancedMesh();
            std::shared_ptr<Shader> fragment = Shader::standardFragmentMesh();
            _standardShaderProgramInstancedMesh = std::make_shared<ShaderProgram>(*vertex, *fragment);
        }
        return _standardShaderProgramInstancedMesh;
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::_standardShaderProgramInstancedMeshOutline = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::standardShaderProgramInstancedMeshOutline() {
        if (_standardShaderProgramInstancedMeshOutline == nullptr) {
            std::shared_ptr<Shader> vertex = Shader::standardVertexInstancedMesh();
            std::shared_ptr<Shader> fragment = Shader::standardFragmentMeshOutline();
            _standardShaderProgramInstancedMeshOutline = std::make_shared<ShaderProgram>(*vertex, *fragment);
        }
        return _standardShaderProgramInstancedMeshOutline;
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::_standardShaderProgramScreenTexture = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::standardShaderProgramScreenTexture() {
        if (_standardShaderProgramScreenTexture == nullptr) {
            std::shared_ptr<Shader> vertex = Shader::standardVertexScreenTexture();
            std::shared_ptr<Shader> fragment = Shader::standardFragmentScreenTexture();
            _standardShaderProgramScreenTexture = std::make_shared<ShaderProgram>(*vertex, *fragment);
        }
        return _standardShaderProgramScreenTexture;
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::_standardShaderProgramSkybox = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::standardShaderProgramSkybox() {
        if (_standardShaderProgramSkybox == nullptr) {
            std::shared_ptr<Shader> vertex = Shader::standardVertexSkybox();
            std::shared_ptr<Shader> fragment = Shader::standardFragmentSkybox();
            _standardShaderProgramSkybox = std::make_shared<ShaderProgram>(*vertex, *fragment);
        }
        return _standardShaderProgramSkybox;
    }

}
