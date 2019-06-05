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

    const int& VoxelChunk::blockAt(int x, int y, int z) const {
        return _blockIds[indexOfBlock(x, y, z)];
    }

    int& VoxelChunk::blockAt(int x, int y, int z) {
        return _blockIds[indexOfBlock(x, y, z)];
    }

    int VoxelChunk::blockIdAt(int x, int y, int z) const {
        return blockAt(x, y, z) & 0xFFFF;
    }

    void VoxelChunk::calculBlockAdjacence(const std::array<std::shared_ptr<VoxelChunk>, 6>& adjChunks) {
        (void)adjChunks;
        for (int x = 0; x < chunkSize; x++)
            for (int y = 0; y < chunkSize; y++)
                for (int z = 0; z < chunkSize; z++) {

                    int blockId = blockIdAt(x, y, z);
                    if (blockId != 0) {

                        int adj = 0;

                        // +x | -x | +y | -y | +z | -z
                        //  0 |  1 |  2 |  3 |  4 |  5

                        // TODO: fix the adjacence bug

                        if ((x >= chunkSize - 1
                                && (adjChunks[0] == nullptr || adjChunks[0]->blockIdAt(0, y, z) == 0))
                            || (blockIdAt(x + 1, y, z) == 0))
                            adj |= (1 << 0);
                        if ((x <= 0
                                && (adjChunks[1] == nullptr || adjChunks[1]->blockIdAt(chunkSize - 1, y, z) == 0))
                            || (blockIdAt(x - 1, y, z) == 0))
                            adj |= (1 << 1);
                        if ((y >= chunkSize - 1
                                && (adjChunks[2] == nullptr || adjChunks[2]->blockIdAt(x, 0, z) == 0))
                            || (blockIdAt(x, y + 1, z) == 0))
                            adj |= (1 << 2);
                        if ((y <= 0
                                && (adjChunks[3] == nullptr || adjChunks[3]->blockIdAt(x, chunkSize - 1, z) == 0))
                            || (blockIdAt(x, y - 1, z) == 0))
                            adj |= (1 << 3);
                        if ((z >= chunkSize - 1
                                && (adjChunks[4] == nullptr || adjChunks[4]->blockIdAt(x, y, 0) == 0))
                            || (blockIdAt(x, y, z + 1) == 0))
                            adj |= (1 << 4);
                        if ((z <= 0
                                && (adjChunks[5] == nullptr || adjChunks[5]->blockIdAt(x, y, chunkSize - 1) == 0))
                            || (blockIdAt(x, y, z - 1) == 0))
                            adj |= (1 << 5);
                        
                        blockAt(x, y, z) = blockId | (adj << 16);

                    } else {
                        blockAt(x, y, z) = 0;
                    }
                }
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

        glVertexAttribPointer(0, 1, GL_INT, GL_FALSE, sizeof(int), NULL);
        glEnableVertexAttribArray(0);

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

    void VoxelChunk::setMaterial(std::shared_ptr<Material> mat) {
        _material = mat;
    }

    std::shared_ptr<Material> VoxelChunk::getMaterial() const {
        return _material;
    }

    void VoxelChunk::resetIdsBufferValues() {
        if (bufferGenerated()) {
            glBindVertexArray(_blocksArray);
            glBindBuffer(GL_ARRAY_BUFFER, _blocksBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(int) * chunkBlockCount, _blockIds, GL_DYNAMIC_DRAW);
        }
    }

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

        glUniformMatrix4fv(program->getLocation("u_mat_projection"), 1, GL_FALSE, glm::value_ptr(uniforms.projection));
        glUniformMatrix4fv(program->getLocation("u_mat_view"), 1, GL_FALSE, glm::value_ptr(uniforms.view));
        glUniformMatrix4fv(program->getLocation("u_mat_model"), 1, GL_FALSE, glm::value_ptr(uniforms.model));
        glUniform3f(program->getLocation("u_camera_position"), uniforms.camera_position.x,
                                                               uniforms.camera_position.y,
                                                               uniforms.camera_position.z);

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
