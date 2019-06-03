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

    VoxelChunk::VoxelChunk() {
        for (int i = 0; i < chunkBlockCount; i++)
            _blockIds[i] = 0;
    }

    VoxelChunk::VoxelChunk(const VoxelChunk& copy) {
        for (int i = 0; i < chunkBlockCount; i++)
            _blockIds[i] = copy._blockIds[i];
    }

    VoxelChunk::~VoxelChunk() {

    }

    VoxelChunk& VoxelChunk::operator=(const VoxelChunk& copy) {
        for (int i = 0; i < chunkBlockCount; i++)
            _blockIds[i] = copy._blockIds[i];
        return (*this);
    }

    std::pair<glm::vec3, glm::vec3> VoxelChunk::getBounds(glm::mat4 transform) const {
        (void)transform;
        return std::pair<glm::vec3, glm::vec3>(0, 0);
    }

    void VoxelChunk::generateBuffers() throw(BufferCreationException) {
        deleteBuffers();
    }

    void VoxelChunk::deleteBuffers() {

    }

    bool VoxelChunk::bufferGenerated() const {
        return true;
    }

    void VoxelChunk::resetIdsBufferValues() {
        
    }

    void VoxelChunk::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
        
        (void)scene;
        (void)uniforms;
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
