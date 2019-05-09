//
//  GLSMesh.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {
    
    InstancedMesh::InstancedMesh() :
    Mesh(),
    _instancesTransforms(1, Transform()),
    _transformsBuffer(0)
    {

    }

    InstancedMesh::InstancedMesh(const Mesh& copy, size_t instanceCount) :
    Mesh(copy),
    _instancesTransforms(instanceCount, Transform()),
    _transformsBuffer(0)
    {

    }

    InstancedMesh::InstancedMesh(const InstancedMesh& copy) :
    Mesh(copy),
    _instancesTransforms(copy._instancesTransforms),
    _transformsBuffer(0)
    {
        if (copy.bufferGenerated())
            generateBuffers();
    }

    InstancedMesh::~InstancedMesh() {

    }

    InstancedMesh& InstancedMesh::operator=(const Mesh& copy) {
        _instancesTransforms.clear();
        _instancesTransforms.push_back(Transform());
        Mesh::operator=(copy);
        return *this;
    }

    InstancedMesh& InstancedMesh::operator=(const InstancedMesh& copy) {
        _instancesTransforms = copy._instancesTransforms;
        Mesh::operator=(copy);
        return *this;
    }

    size_t InstancedMesh::instancesCount() const {
        return _instancesTransforms.size();
    }

    void InstancedMesh::setInstancesCount(size_t count) {
        // TODO: there may be a better way...
        if (count < _instancesTransforms.size()) {
            while (count < _instancesTransforms.size()) {
                _instancesTransforms.pop_back();
            }
        } else if (count > _instancesTransforms.size()) {
            while (count > _instancesTransforms.size()) {
                _instancesTransforms.push_back(Transform());
            }
        }
        _bufferGenerated = false;
    }

    const Transform& InstancedMesh::instanceTransformAt(size_t i) const {
        return _instancesTransforms[i];
    }

    void InstancedMesh::setInstanceTransformAt(size_t i, Transform t) {
        _instancesTransforms[i] = t;
        _bufferGenerated = false;
    }

    void InstancedMesh::generateBuffers() throw(BufferCreationException) {
        Mesh::generateBuffers();
        if (!_bufferGenerated)
            return;

        glGenBuffers(1, &_transformsBuffer);
        if (_transformsBuffer == 0) {
            Mesh::deleteBuffers();
            throw BufferCreationException();
        }

        const size_t instancesCount = _instancesTransforms.size();
        glm::mat4 *modelMatrices = new glm::mat4[instancesCount];
        if (modelMatrices == nullptr) {
            deleteBuffers();
            throw BufferCreationException(); // yes... no... fuck you...
        }
        for (size_t i = 0; i < instancesCount; i++)
            modelMatrices[i] = _instancesTransforms[i].matrix();
        
        glBindBuffer(GL_ARRAY_BUFFER, _transformsBuffer);
        glBufferData(GL_ARRAY_BUFFER, _instancesTransforms.size() * sizeof(glm::mat4), modelMatrices, GL_STATIC_DRAW);
        delete[] modelMatrices;

        glBindVertexArray(_elementsBuffer);
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(7, 1);
        glVertexAttribDivisor(8, 1);
        glBindVertexArray(0);

    }

    void InstancedMesh::deleteBuffers() {
        Mesh::deleteBuffers();
        if (_transformsBuffer != 0)
            glDeleteBuffers(1, &_transformsBuffer);
        _transformsBuffer = 0;
    }

    void InstancedMesh::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
        if (!_bufferGenerated)
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

        glm::mat3 normalMatrix = glm::inverseTranspose(uniforms.model);
        glUniformMatrix4fv(program->getLocation("u_mat_projection"), 1, GL_FALSE, glm::value_ptr(uniforms.projection));
        glUniformMatrix3fv(program->getLocation("u_mat_normal"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniformMatrix4fv(program->getLocation("u_mat_view"), 1, GL_FALSE, glm::value_ptr(uniforms.view));
        glUniformMatrix4fv(program->getLocation("u_mat_model"), 1, GL_FALSE, glm::value_ptr(uniforms.model));
        glUniform3f(program->getLocation("u_camera_position"), uniforms.camera_position.x,
                                                               uniforms.camera_position.y,
                                                               uniforms.camera_position.z);

        if (_outlined) {
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            scene.subscribeToPostRenderable(this, uniforms, 1);
        } else {
            glStencilMask(0x00);
        }

        _material->sendUniformToShaderProgram(program);
        glBindVertexArray(_elementsBuffer);
        glDrawElementsInstanced(GL_TRIANGLES,
                                static_cast<GLsizei>(_indices.size()),
                                GL_UNSIGNED_INT, 0, _instancesTransforms.size());
        
    }
    
    void InstancedMesh::postRenderInContext(Scene& scene, const RenderUniforms& uniforms, float priority) {

        if (_outlined && priority == 1) {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramInstancedMeshOutline();
            program->use();
            glm::mat4 scaleUpModel = glm::scale(uniforms.model, glm::vec3(_outlineSize));

            glUniformMatrix4fv(program->getLocation("u_mat_projection"), 1, GL_FALSE, glm::value_ptr(uniforms.projection));
            glUniformMatrix4fv(program->getLocation("u_mat_view"), 1, GL_FALSE, glm::value_ptr(uniforms.view));
            glUniformMatrix4fv(program->getLocation("u_mat_model"), 1, GL_FALSE, glm::value_ptr(scaleUpModel));
            glUniform3f(program->getLocation("border_color"), _outlineColor.x, _outlineColor.y, _outlineColor.z);

            glBindVertexArray(_elementsBuffer);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, 0, _instancesTransforms.size());
            glStencilMask(0xFF);
        }
        (void)scene;
        (void)priority;
    }

}
