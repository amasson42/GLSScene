//
//  GLSVoxelChunk.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSVoxelChunk.hpp"

namespace GLS {

    int VoxelChunk::indexOfBlock(int x, int y, int z) {
        return (z * chunkSize * chunkSize + y * chunkSize + x);
    }

    std::tuple<int, int, int> VoxelChunk::coordinatesOfBlock(int i) {
        return std::make_tuple(i % chunkSize,
                                (i % (chunkSize * chunkSize)) / chunkSize,
                                i / (chunkSize * chunkSize));
    }

    const char *VoxelChunk::BufferCreationException::what() const throw() {
        return "could not create additional buffer";
    }

    VoxelChunk::VoxelChunk() :
    _blocksBuffer(0), _blocksArray(0)
    {
        for (int i = 0; i < chunkBlockCount; i++)
            _blockIds[i] = 0;
    }

    VoxelChunk::VoxelChunk(const VoxelChunk& copy) :
    _blocksBuffer(0), _blocksArray(0)
    {
        for (int i = 0; i < chunkBlockCount; i++)
            _blockIds[i] = copy._blockIds[i];
        if (copy.bufferGenerated())
            generateBuffers();
    }

    VoxelChunk::~VoxelChunk() {
        deleteBuffers();
    }

    VoxelChunk& VoxelChunk::operator=(const VoxelChunk& copy) {
        for (int i = 0; i < chunkBlockCount; i++)
            _blockIds[i] = copy._blockIds[i];
        if (copy.bufferGenerated())
            generateBuffers();
        return (*this);
    }

    int *VoxelChunk::blockIds() {
        return _blockIds;
    }

    std::pair<glm::vec3, glm::vec3> VoxelChunk::getBounds(glm::mat4 transform) const {
        (void)transform;
        return std::pair<glm::vec3, glm::vec3>(0, 0);
    }

    void VoxelChunk::generateBuffers() throw(BufferCreationException) {
        deleteBuffers();

        glGenVertexArrays(1, &_blocksArray);
        if (_blocksArray == 0)
            throw BufferCreationException();
        glBindVertexArray(_blocksArray);

        glGenBuffers(1, &_blocksBuffer);
        if (_blocksBuffer == 0) {
            glDeleteVertexArrays(1, &_blocksArray);
            throw BufferCreationException();
        }
        glBindBuffer(GL_ARRAY_BUFFER, _blocksBuffer);

        resetIdsBufferValues();
    }

    void VoxelChunk::deleteBuffers() {
        if (_blocksBuffer != 0)
            glDeleteBuffers(1, &_blocksBuffer);
        _blocksBuffer = 0;
        if (_blocksArray != 0)
            glDeleteVertexArrays(1, &_blocksArray);
        _blocksArray = 0;
    }

    bool VoxelChunk::bufferGenerated() const {
        return _blocksBuffer != 0 && _blocksArray != 0;
    }

    void VoxelChunk::resetIdsBufferValues() {
        if (bufferGenerated()) {
            glBindVertexArray(_blocksArray);
            glBindBuffer(GL_ARRAY_BUFFER, _blocksBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(int) * chunkBlockCount, _blockIds, GL_DYNAMIC_DRAW);
        }
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

        glUniformMatrix4fv(program->getLocation("u_mat_projection"), 1, GL_FALSE, glm::value_ptr(uniforms.projection));
        glUniformMatrix4fv(program->getLocation("u_mat_view"), 1, GL_FALSE, glm::value_ptr(uniforms.view));
        glUniformMatrix4fv(program->getLocation("u_mat_model"), 1, GL_FALSE, glm::value_ptr(uniforms.model));
        glUniform3f(program->getLocation("u_camera_position"), uniforms.camera_position.x,
                                                               uniforms.camera_position.y,
                                                               uniforms.camera_position.z);

        glStencilMask(0x00);

        static std::shared_ptr<Material> mat = nullptr;
        if (mat == nullptr) {
            mat = std::make_shared<Material>();
            mat->texture_diffuse = std::make_shared<Texture>("../textures/container.jpg");
        }
        mat->sendUniformToShaderProgram(program);

        glBindVertexArray(_blocksArray);
        glDrawArrays(GL_POINTS, 0, chunkBlockCount);
    }

    void VoxelChunk::postRenderInContext(Scene& scene, const RenderUniforms& uniforms, float priority) {
        (void)scene;
        (void)uniforms;
        (void)priority;
    }

    void VoxelChunk::renderInDepthContext(Scene& scene, const RenderUniforms& uniforms) {
        (void)scene;
        (void)uniforms;
    }
}
