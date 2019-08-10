//
//  GLSInstancedMesh_render.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {

    void InstancedMesh::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
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
            program = ShaderProgram::standardShaderProgramInstancedMesh();
            program->use();
        }
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        uniforms.sendUniformsToShaderProgram(program);

        if (_outlined) {
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            scene.subscribeToPostRenderable(this, uniforms, 1);
        } else {
            glStencilMask(0x00);
        }

        if (_material == nullptr)
            Material().sendUniformsToShaderProgram(program);
        else
            _material->sendUniformsToShaderProgram(program);

        glBindVertexArray(_elementsBuffer);
        glDrawElementsInstanced(_drawMode,
                                static_cast<GLsizei>(_indices.size()),
                                GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_instancesTransforms.size()));
    }
    
    void InstancedMesh::postRenderInContext(Scene& scene, const RenderUniforms& uniforms, float priority) {
        (void)scene;

        if (_outlined && priority == 1) {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramInstancedMeshSimpleColor();
            program->use();

            RenderUniforms scaledUniforms(uniforms);
            scaledUniforms.model = glm::scale(uniforms.model, glm::vec3(_outlineSize));
            scaledUniforms.sendUniformsToShaderProgram(program);
            glUniform3f(program->getLocation("material.diffuse"), _outlineColor.x, _outlineColor.y, _outlineColor.z);

            glBindVertexArray(_elementsBuffer);
            glDrawElementsInstanced(_drawMode, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_instancesTransforms.size()));
            glStencilMask(0xFF);
        }
    }

    void InstancedMesh::renderInDepthContext(Scene& scene, const RenderUniforms& uniforms) {
        (void)scene;
        if (!bufferGenerated())
            generateBuffers();
        if (!bufferGenerated())
            return;

        std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramInstancedMeshSimpleColor();
        program->use();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        uniforms.sendUniformsToShaderProgram(program);

        glBindVertexArray(_elementsBuffer);
        glDrawElementsInstanced(_drawMode,
                                static_cast<GLsizei>(_indices.size()),
                                GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_instancesTransforms.size()));
    }

}
