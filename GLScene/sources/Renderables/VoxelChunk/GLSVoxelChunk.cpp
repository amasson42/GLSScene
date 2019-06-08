//
//  GLSVoxelChunk.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSVoxelChunk.hpp"

namespace GLS {

    const char *VoxelChunk::BufferCreationException::what() const throw() {
        return "could not create additional buffer";
    }

    VoxelChunk::VoxelChunk() :
    _blocksBuffer(0), _blocksArray(0),
    _shaderProgram(nullptr),
    _material(nullptr),
    _adjChunks(),
    _isEmpty(true)
    {
        for (int i = 0; i < chunkBlockCount; i++)
            _blockIds[i] = 0;
        for (int i = 0; i < 6; i++)
            _fullEdges[i] = false;
    }

    VoxelChunk::VoxelChunk(const VoxelChunk& copy) :
    _blocksBuffer(0), _blocksArray(0),
    _shaderProgram(nullptr),
    _material(copy._material),
    _adjChunks(),
    _isEmpty(copy._isEmpty)
    {
        for (int i = 0; i < chunkBlockCount; i++)
            _blockIds[i] = copy._blockIds[i];
        if (copy.bufferGenerated())
            generateBuffers();
        for (int i = 0; i < 6; i++)
            _fullEdges[i] = copy._fullEdges[i];
    }

    VoxelChunk::~VoxelChunk() {
        deleteBuffers();
    }

    VoxelChunk& VoxelChunk::operator=(const VoxelChunk& copy) {
        for (int i = 0; i < chunkBlockCount; i++)
            _blockIds[i] = copy._blockIds[i];
        deleteBuffers();
        _material = copy._material;
        _isEmpty = copy._isEmpty;
        for (int i = 0; i < 6; i++)
            _fullEdges[i] = copy._fullEdges[i];
        if (copy.bufferGenerated())
            generateBuffers();
        return (*this);
    }

}
