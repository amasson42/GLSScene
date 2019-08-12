//
//  GLSMesh_render.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {

    void Mesh::setProgram(std::shared_ptr<ShaderProgram> shaderProgram) {
        _shaderProgram = shaderProgram;
    }

    void Mesh::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
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
            program = ShaderProgram::standardShaderProgramMesh();
            program->use();
        }
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(uniforms.model));
        glUniformMatrix3fv(program->getLocation("u_mat_normal"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
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
        glDrawElements(_drawMode,
                       static_cast<GLsizei>(_indices.size()),
                       GL_UNSIGNED_INT, 0);
        
    }
    
    void Mesh::postRenderInContext(Scene& scene, const RenderUniforms& uniforms, float priority) {
        (void)scene;
        if (_outlined && priority == 1) {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramMeshSimpleColor();
            program->use();

            RenderUniforms scaledUniforms(uniforms);
            scaledUniforms.model = glm::scale(uniforms.model, glm::vec3(_outlineSize));
            scaledUniforms.sendUniformsToShaderProgram(program);
            glUniform3f(program->getLocation("material.diffuse"), _outlineColor.x, _outlineColor.y, _outlineColor.z);

            glBindVertexArray(_elementsBuffer);
            glDrawElements(_drawMode, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, 0);
            glStencilMask(0xFF);
        }
    }

    void Mesh::renderInDepthContext(Scene& scene, const RenderUniforms& uniforms) {
        (void)scene;
        if (!bufferGenerated())
            generateBuffers();
        if (!bufferGenerated())
            return;

        std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramMeshSimpleColor();
        program->use();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        uniforms.sendUniformsToShaderProgram(program);

        glBindVertexArray(_elementsBuffer);
        glDrawElements(_drawMode,
                       static_cast<GLsizei>(_indices.size()),
                       GL_UNSIGNED_INT, 0);
    }

}
