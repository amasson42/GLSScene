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
        _instancesTransforms.resize(count, Transform());
        deleteBuffers();
    }

    const Transform& InstancedMesh::instanceTransformAt(size_t i) const {
        return _instancesTransforms[i];
    }

    void InstancedMesh::setInstanceTransformAt(size_t i, Transform t) {
        _instancesTransforms[i] = t;
    }

    void InstancedMesh::generateBuffers() throw(BufferCreationException) {
        Mesh::generateBuffers();
        if (!Mesh::bufferGenerated())
            return;

        glGenBuffers(1, &_transformsBuffer);
        if (_transformsBuffer == 0) {
            Mesh::deleteBuffers();
            throw BufferCreationException();
        }

        glBindBuffer(GL_ARRAY_BUFFER, _transformsBuffer);

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

        updateTransformsBufferValues();
    }

    void InstancedMesh::deleteBuffers() {
        Mesh::deleteBuffers();
        if (_transformsBuffer != 0)
            glDeleteBuffers(1, &_transformsBuffer);
        _transformsBuffer = 0;
    }

    bool InstancedMesh::bufferGenerated() const {
        return (Mesh::bufferGenerated() && _transformsBuffer != 0);
    }

    void InstancedMesh::updateTransformsBufferValues() {
        if (bufferGenerated()) {
            glBindVertexArray(_elementsBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, _transformsBuffer);
            const size_t instancesCount = _instancesTransforms.size();
            glm::mat4 *modelMatrices = new glm::mat4[instancesCount];
            for (size_t i = 0; i < instancesCount; i++)
                modelMatrices[i] = _instancesTransforms[i].matrix();
            glBufferData(GL_ARRAY_BUFFER, _instancesTransforms.size() * sizeof(glm::mat4), modelMatrices, GL_DYNAMIC_DRAW);
            delete [] modelMatrices;
        }
    }

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

        glUniformMatrix4fv(program->getLocation("u_mat_projection"), 1, GL_FALSE, glm::value_ptr(uniforms.projection));
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

        if (_material == nullptr)
            Material().sendUniformToShaderProgram(program);
        else
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
            std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramInstancedMeshSimpleColor();
            program->use();

            glm::mat4 scaleUpModel = glm::scale(uniforms.model, glm::vec3(_outlineSize));

            glUniformMatrix4fv(program->getLocation("u_mat_projection"), 1, GL_FALSE, glm::value_ptr(uniforms.projection));
            glUniformMatrix4fv(program->getLocation("u_mat_view"), 1, GL_FALSE, glm::value_ptr(uniforms.view));
            glUniformMatrix4fv(program->getLocation("u_mat_model"), 1, GL_FALSE, glm::value_ptr(scaleUpModel));
            glUniform3f(program->getLocation("color"), _outlineColor.x, _outlineColor.y, _outlineColor.z);

            glBindVertexArray(_elementsBuffer);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, 0, _instancesTransforms.size());
            glStencilMask(0xFF);
        }
        (void)scene;
    }

    void InstancedMesh::renderInDepthContext(Scene& scene, const RenderUniforms& uniforms) {
        if (!bufferGenerated())
            generateBuffers();
        if (!bufferGenerated())
            return;

        std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramInstancedMeshSimpleColor();
        program->use();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        glUniformMatrix4fv(program->getLocation("u_mat_projection"), 1, GL_FALSE, glm::value_ptr(uniforms.projection));
        glUniformMatrix4fv(program->getLocation("u_mat_view"), 1, GL_FALSE, glm::value_ptr(uniforms.view));
        glUniformMatrix4fv(program->getLocation("u_mat_model"), 1, GL_FALSE, glm::value_ptr(uniforms.model));

        glBindVertexArray(_elementsBuffer);
        glDrawElementsInstanced(GL_TRIANGLES,
                                static_cast<GLsizei>(_indices.size()),
                                GL_UNSIGNED_INT, 0, _instancesTransforms.size());
        (void)scene;
    }

    std::string InstancedMesh::shaderUniformsVertex() {
        return
        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec3 normal;\n"
        "layout (location = 2) in vec3 tangent;\n"
        "layout (location = 3) in vec3 bitangent;\n"
        "layout (location = 4) in vec2 uv;\n"
        "layout (location = 5) in mat4 model_matrix;\n"
        "\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "\n"
        "out VS_OUT {\n"
        "    vec4 position;\n"
        "    vec3 wposition;\n"
        "    vec2 uv;\n"
        "    vec3 wnormal;\n"
        "    vec3 wtangent;\n"
        "    vec3 wbitangent;\n"
        "} vs_out;\n"
        "\n";
    }

    std::string InstancedMesh::shaderUniformsGeometry() {
        return
        "in VS_OUT {\n"
        "    vec4 position;\n"
        "    vec3 wposition;\n"
        "    vec2 uv;\n"
        "    vec3 wnormal;\n"
        "    vec3 wtangent;\n"
        "    vec3 wbitangent;\n"
        "} gs_in;\n"
        "\n"
        "out VS_OUT {\n"
        "    vec4 position;\n"
        "    vec3 wposition;\n"
        "    vec2 uv;\n"
        "    vec3 wnormal;\n"
        "    vec3 wtangent;\n"
        "    vec3 wbitangent;\n"
        "} gs_out;\n"
        "\n";
    }

    std::string InstancedMesh::shaderUniformsFragment() {
        return Mesh::shaderUniformsFragment();
    }

}
