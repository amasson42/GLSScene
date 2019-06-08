//
//  GLSVoxelChunk_buffers.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSVoxelChunk.hpp"

namespace GLS {

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

        updateIdsBuffer();
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

    void VoxelChunk::updateIdsBuffer() {
        if (bufferGenerated()) {
            glBindVertexArray(_blocksArray);
            glBindBuffer(GL_ARRAY_BUFFER, _blocksBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(int) * chunkBlockCount, _blockIds, GL_DYNAMIC_DRAW);
        }
    }

}