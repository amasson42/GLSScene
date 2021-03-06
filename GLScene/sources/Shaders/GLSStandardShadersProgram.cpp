//
//  GLSStandardShadersProgram.cpp
//  GLScene
//
//  Created by Arthur MASSON on 4/27/18.
//  Copyright © 2018 Arthur MASSON. All rights reserved.
//

#include "GLSShader.hpp"

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

    std::shared_ptr<ShaderProgram> ShaderProgram::_standardShaderProgramMeshSimpleColor = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::standardShaderProgramMeshSimpleColor() {
        if (_standardShaderProgramMeshSimpleColor == nullptr) {
            std::shared_ptr<Shader> vertex = Shader::standardVertexMesh();
            std::shared_ptr<Shader> fragment = Shader::standardFragmentMeshSimpleColor();
            _standardShaderProgramMeshSimpleColor = std::make_shared<ShaderProgram>(*vertex, *fragment);
        }
        return _standardShaderProgramMeshSimpleColor;
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

    std::shared_ptr<ShaderProgram> ShaderProgram::_standardShaderProgramInstancedMeshSimpleColor = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::standardShaderProgramInstancedMeshSimpleColor() {
        if (_standardShaderProgramInstancedMeshSimpleColor == nullptr) {
            std::shared_ptr<Shader> vertex = Shader::standardVertexInstancedMesh();
            std::shared_ptr<Shader> fragment = Shader::standardFragmentMeshSimpleColor();
            _standardShaderProgramInstancedMeshSimpleColor = std::make_shared<ShaderProgram>(*vertex, *fragment);
        }
        return _standardShaderProgramInstancedMeshSimpleColor;
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::_standardShaderProgramSkinnedMesh = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::standardShaderProgramSkinnedMesh() {
        if (_standardShaderProgramSkinnedMesh == nullptr) {
            std::shared_ptr<Shader> vertex = Shader::standardVertexSkinnedMesh();
            std::shared_ptr<Shader> fragment = Shader::standardFragmentMesh();
            _standardShaderProgramSkinnedMesh = std::make_shared<ShaderProgram>(*vertex, *fragment);
        }
        return _standardShaderProgramSkinnedMesh;
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::_standardShaderProgramSkinnedMeshSimpleColor = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::standardShaderProgramSkinnedMeshSimpleColor() {
        if (_standardShaderProgramSkinnedMeshSimpleColor == nullptr) {
            std::shared_ptr<Shader> vertex = Shader::standardVertexSkinnedMesh();
            std::shared_ptr<Shader> fragment = Shader::standardFragmentMeshSimpleColor();
            _standardShaderProgramSkinnedMeshSimpleColor = std::make_shared<ShaderProgram>(*vertex, *fragment);
        }
        return _standardShaderProgramSkinnedMeshSimpleColor;
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

    std::shared_ptr<ShaderProgram> ShaderProgram::_standardShaderProgramDotParticleSystem = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::standardShaderProgramDotParticleSystem() {
        if (_standardShaderProgramDotParticleSystem == nullptr) {
            std::shared_ptr<Shader> vertex = Shader::standardVertexParticleSystem();
            std::shared_ptr<Shader> fragment = Shader::standardFragmentDotParticleSystem();
            _standardShaderProgramDotParticleSystem = std::make_shared<ShaderProgram>(*vertex, *fragment);
        }
        return _standardShaderProgramDotParticleSystem;
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::_standardShaderProgramTexturedParticleSystem = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::standardShaderProgramTexturedParticleSystem() {
        if (_standardShaderProgramTexturedParticleSystem == nullptr) {
            std::shared_ptr<Shader> vertex = Shader::standardVertexParticleSystem();
            std::shared_ptr<Shader> geometry = Shader::standardGeometryTexturedParticleSystem();
            std::shared_ptr<Shader> fragment = Shader::standardFragmentTexturedParticleSystem();
            _standardShaderProgramTexturedParticleSystem = std::make_shared<ShaderProgram>(*vertex, *geometry, *fragment);
        }
        return _standardShaderProgramTexturedParticleSystem;
    }

}
