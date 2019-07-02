//
//  GLSSkinnedMesh_buffers.cpp
//  GLScene
//
//  Created by Arthur Masson on 26/06/2019.
//  Copyright © 2019 Arthur Masson. All rights reserved.
//

#include "GLSSkinnedMesh.hpp"

namespace GLS {

    void SkinnedMesh::generateBuffers() {
        deleteBuffers();

        glGenVertexArrays(1, &_elementsBuffer);
        if (_elementsBuffer == 0)
            throw GLObjectCreationException(GLOBJECT_VERTEXARRAY);
        glGenBuffers(1, &_verticesBuffer);
        if (_verticesBuffer == 0) {
            glDeleteVertexArrays(1, &_elementsBuffer);
            throw GLObjectCreationException(GLOBJECT_BUFFER);
        }
        glGenBuffers(1, &_indicesBuffer);
        if (_indicesBuffer == 0) {
            glDeleteVertexArrays(1, &_elementsBuffer);
            glDeleteBuffers(1, &_verticesBuffer);
            throw GLObjectCreationException(GLOBJECT_BUFFER);
        }

        glBindVertexArray(_elementsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _verticesBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SkinnedVertex) * _vertices.size(),
                     _vertices.empty() ? NULL : &_vertices.front(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _indices.size(),
                     _indices.empty() ? NULL : &_indices.front(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              sizeof(SkinnedVertex), (void*)(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                              sizeof(SkinnedVertex), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                              sizeof(SkinnedVertex), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                              sizeof(SkinnedVertex), (void*)(9 * sizeof(GLfloat)));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE,
                              sizeof(SkinnedVertex), (void*)(12 * sizeof(GLfloat)));
        glEnableVertexAttribArray(4);

        glVertexAttribIPointer(5, 4, GL_INT,
                              sizeof(SkinnedVertex), (void*)(14 * sizeof(GLfloat)));
        glEnableVertexAttribArray(5);

        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE,
                              sizeof(SkinnedVertex), (void*)(14 * sizeof(GLfloat) + 4 + sizeof(GLint)));
        glEnableVertexAttribArray(6);
    }

    void SkinnedMesh::deleteBuffers() {
        if (_elementsBuffer)
            glDeleteVertexArrays(1, &_elementsBuffer);
        _elementsBuffer = 0;
        if (_verticesBuffer)
            glDeleteBuffers(1, &_verticesBuffer);
        _verticesBuffer = 0;
        if (_indicesBuffer)
            glDeleteBuffers(1, &_indicesBuffer);
        _indicesBuffer = 0;
    }

    bool SkinnedMesh::bufferGenerated() const {
        return _elementsBuffer != 0 && _verticesBuffer != 0 && _indicesBuffer != 0;
    }

}
