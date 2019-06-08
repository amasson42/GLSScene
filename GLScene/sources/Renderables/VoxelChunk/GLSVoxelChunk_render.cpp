//
//  GLSVoxelChunk_render.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSVoxelChunk.hpp"

namespace GLS {

    void VoxelChunk::setProgram(std::shared_ptr<ShaderProgram> shaderProgram) {
        _shaderProgram = shaderProgram;
    }

    void VoxelChunk::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
        if (!bufferGenerated())
            generateBuffers();
        if (!bufferGenerated())
            return;
        
        std::shared_ptr<ShaderProgram> program;
        if (_shaderProgram) {
            program = _shaderProgram;
            program->use();
            scene.sendLightsValueToShader(program);
        } else {
            program = ShaderProgram::standardShaderProgramVoxelChunk();
            program->use();
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        uniforms.sendUniformsToShaderProgram(program);

        glStencilMask(0x00);

        if (_material == nullptr)
            Material().sendUniformToShaderProgram(program);
        else
            _material->sendUniformToShaderProgram(program);

        glBindVertexArray(_blocksArray);
        glDrawArrays(GL_POINTS, 0, chunkBlockCount);
    }

    void VoxelChunk::renderInDepthContext(Scene& scene, const RenderUniforms& uniforms) {
        if (!bufferGenerated())
            generateBuffers();
        if (!bufferGenerated())
            return;
        
        std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramVoxelChunkSimpleColor();
        program->use();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        glUniformMatrix4fv(program->getLocation("u_mat_projection"), 1, GL_FALSE, glm::value_ptr(uniforms.projection));
        glUniformMatrix4fv(program->getLocation("u_mat_view"), 1, GL_FALSE, glm::value_ptr(uniforms.view));
        glUniformMatrix4fv(program->getLocation("u_mat_model"), 1, GL_FALSE, glm::value_ptr(uniforms.model));

        glBindVertexArray(_blocksArray);
        glDrawArrays(GL_POINTS, 0, chunkBlockCount);
        (void)scene;
    }

}
