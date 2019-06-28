//
//  GLSInstanced_buffers.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {

    void InstancedMesh::generateBuffers() {
        Mesh::generateBuffers();
        if (!Mesh::bufferGenerated())
            return;

        glGenBuffers(1, &_transformsBuffer);
        if (_transformsBuffer == 0) {
            Mesh::deleteBuffers();
            throw GLObjectCreationException(GLOBJECT_BUFFER);
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

}
