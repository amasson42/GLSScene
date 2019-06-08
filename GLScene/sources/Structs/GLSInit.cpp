//
//  GLSInit.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLScene.hpp"

namespace GLS {

    void glsInit() {
        Framebuffer::_createRectBuffer();
        Skybox::_createCubeBuffer();
        
        ShaderProgram::standardShaderProgramMesh();
        ShaderProgram::standardShaderProgramMeshSimpleColor();
        ShaderProgram::standardShaderProgramInstancedMesh();
        ShaderProgram::standardShaderProgramInstancedMeshSimpleColor();
        ShaderProgram::standardShaderProgramScreenTexture();
        ShaderProgram::standardShaderProgramSkybox();
        ShaderProgram::standardShaderProgramVoxelChunk();
        ShaderProgram::standardShaderProgramVoxelChunkSimpleColor();
        ShaderProgram::standardShaderProgramDotParticleSystem();
        ShaderProgram::standardShaderProgramTexturedParticleSystem();
    }

    void glsDeinit() {
        Framebuffer::_destroyRectBuffer();
        Skybox::_destroyCubeBuffer();

        ShaderProgram::_standardShaderProgramMesh = nullptr;
        ShaderProgram::_standardShaderProgramMeshSimpleColor = nullptr;
        ShaderProgram::_standardShaderProgramInstancedMesh = nullptr;
        ShaderProgram::_standardShaderProgramInstancedMeshSimpleColor = nullptr;
        ShaderProgram::_standardShaderProgramScreenTexture = nullptr;
        ShaderProgram::_standardShaderProgramSkybox = nullptr;
        ShaderProgram::_standardShaderProgramVoxelChunk = nullptr;
        ShaderProgram::_standardShaderProgramVoxelChunkSimpleColor = nullptr;
        ShaderProgram::_standardShaderProgramDotParticleSystem = nullptr;
        ShaderProgram::_standardShaderProgramTexturedParticleSystem = nullptr;
    }

}
