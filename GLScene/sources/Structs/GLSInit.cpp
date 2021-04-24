//
//  GLSInit.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLScene.hpp"

namespace GLS {

    static std::shared_ptr<CLD::GPUDevice> _gpuDevice = nullptr;

    std::shared_ptr<CLD::GPUDevice> getSharedDevice() {
        return _gpuDevice;
    }

    void glsInit(void* addr) {

        _gpuDevice = std::make_shared<CLD::GPUDevice>();
        _gpuDevice->createContext(true);

        #ifdef __linux__
        glewInit();
        #endif

		#ifdef WIN32
		if (!gladLoadGLLoader((GLADloadproc)addr)) {
			std::cout << "Glad Init Error" << std::endl;
			exit(EXIT_FAILURE);
		}
        #else
        (void)addr;
		#endif

		Framebuffer::_createRectBuffer();
		Skybox::_createCubeBuffer();
		ParticleSystem::_createDevice();
        
        ShaderProgram::standardShaderProgramMesh();
        ShaderProgram::standardShaderProgramMeshSimpleColor();
        ShaderProgram::standardShaderProgramInstancedMesh();
        ShaderProgram::standardShaderProgramInstancedMeshSimpleColor();
        ShaderProgram::standardShaderProgramSkinnedMesh();
        ShaderProgram::standardShaderProgramSkinnedMeshSimpleColor();
        ShaderProgram::standardShaderProgramScreenTexture();
        ShaderProgram::standardShaderProgramSkybox();
        ShaderProgram::standardShaderProgramDotParticleSystem();
        ShaderProgram::standardShaderProgramTexturedParticleSystem();

        Node::_assimpImporter = std::make_shared<Assimp::Importer>();
    }

    void glsDeinit() {
        Framebuffer::_destroyRectBuffer();
        Skybox::_destroyCubeBuffer();
        ParticleSystem::_destroyDevice();

        ShaderProgram::_standardShaderProgramMesh = nullptr;
        ShaderProgram::_standardShaderProgramMeshSimpleColor = nullptr;
        ShaderProgram::_standardShaderProgramInstancedMesh = nullptr;
        ShaderProgram::_standardShaderProgramInstancedMeshSimpleColor = nullptr;
        ShaderProgram::_standardShaderProgramSkinnedMesh = nullptr;
        ShaderProgram::_standardShaderProgramSkinnedMeshSimpleColor = nullptr;
        ShaderProgram::_standardShaderProgramScreenTexture = nullptr;
        ShaderProgram::_standardShaderProgramSkybox = nullptr;
        ShaderProgram::_standardShaderProgramDotParticleSystem = nullptr;
        ShaderProgram::_standardShaderProgramTexturedParticleSystem = nullptr;

        Node::_assimpImporter = nullptr;

        _gpuDevice = nullptr;
    }

}
