//
//  GLSSkinnedMesh_render.cpp
//  GLScene
//
//  Created by Arthur Masson on 26/06/2019.
//  Copyright © 2019 Arthur Masson. All rights reserved.
//

#include "GLSSkinnedMesh.hpp"
#include "GLSScene.hpp"

namespace GLS {

    void SkinnedMesh::setProgram(std::shared_ptr<ShaderProgram> shaderProgram) {
        _shaderProgram = shaderProgram;
    }

    void SkinnedMesh::_sendBonesToShaderProgram(std::shared_ptr<ShaderProgram> program) {
        program->use();
        for (size_t i = 0; i < _skeleton->bones().size() && i < Skeleton::maxBones; i++) {

            const Skeleton::Bone& bone(_skeleton->bones()[i]);

            if (bone.node.expired()) {

                // TODO: remove this when everything works
                static bool expired_bone_once = true;
                if (expired_bone_once) {
                    std::cout << "expired bone " << i << std::endl;
                    expired_bone_once = false;
                }

                continue;
            }

            std::shared_ptr<Node> node = bone.node.lock();
            // std::shared_ptr<Node> rootNode = _skeleton->bones()[0].node.lock();
            std::shared_ptr<Node> rootNode = _rootNode.lock();
            
            glm::mat4 rootRelative = node->getParentNodeRelativeTransformMatrix(rootNode);
            // glm::mat4 rootRelative = node->getWorldTransformMatrix();

            glm::mat4 modelMatrix = glm::inverse(bone.globalRestPosition) * rootRelative;// * bone.offset;
            // modelMatrix = rootRelative;

            // FIXME: what the maths ?
            // modelMatrix = glm::mat4(1);

            glUniformMatrix4fv(program->getLocation("u_mat_joints[" + std::to_string(i) + "]"),
                1, GL_TRUE, glm::value_ptr(modelMatrix));
        }
    }

    void SkinnedMesh::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
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
            program = ShaderProgram::standardShaderProgramSkinnedMesh();
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

        _sendBonesToShaderProgram(program);

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

    void SkinnedMesh::renderInDepthContext(Scene& scene, const RenderUniforms& uniforms) {
        (void)scene;
        if (!bufferGenerated())
            generateBuffers();
        if (!bufferGenerated())
            return;

        std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramSkinnedMeshSimpleColor();
        program->use();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        uniforms.sendUniformsToShaderProgram(program);
		if (_material != nullptr && _material->texture_mask != nullptr) {
			_material->texture_mask->sendUniformsToShaderProgram(program, "texture_mask", 1);
			glUniform1i(program->getLocation("has_texturemask"), 1);
		} else {
			glUniform1i(program->getLocation("has_texturemask"), 0);
		}

        _sendBonesToShaderProgram(program);

        glBindVertexArray(_elementsBuffer);
        glDrawElements(_drawMode,
                       static_cast<GLsizei>(_indices.size()),
                       GL_UNSIGNED_INT, 0);
    }

    void SkinnedMesh::postRenderInContext(Scene& scene, const RenderUniforms& uniforms, float priority) {
        (void)scene;
        if (_outlined && priority == 1) {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramSkinnedMeshSimpleColor();
            program->use();

            RenderUniforms scaleUniforms(uniforms);
            scaleUniforms.model = glm::scale(uniforms.model, glm::vec3(_outlineSize));
            scaleUniforms.sendUniformsToShaderProgram(program);
            glUniform3f(program->getLocation("material.diffuse"), _outlineColor.x, _outlineColor.y, _outlineColor.z);

            _sendBonesToShaderProgram(program);

            glBindVertexArray(_elementsBuffer);
            glDrawElements(_drawMode, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, 0);
            glStencilMask(0xFF);
        }
    }

}
