//
//  GLSMesh_buffers.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {

    void Mesh::generateBuffers() throw(BufferCreationException) {
        deleteBuffers();
        if (_vertices.empty() || _indices.empty())
            return ;

        glGenVertexArrays(1, &_elementsBuffer);
        if (_elementsBuffer == 0)
            throw BufferCreationException();
        glGenBuffers(1, &_verticesBuffer);
        if (_verticesBuffer == 0) {
            glDeleteVertexArrays(1, &_elementsBuffer);
            throw BufferCreationException();
        }
        glGenBuffers(1, &_indicesBuffer);
        if (_indicesBuffer == 0) {
            glDeleteVertexArrays(1, &_elementsBuffer);
            glDeleteBuffers(1, &_verticesBuffer);
            throw BufferCreationException();
        }

        glBindVertexArray(_elementsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _verticesBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _vertices.size(),
                     &_vertices.front(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _indices.size(),
                     &_indices.front(), GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void*)(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void*)(9 * sizeof(GLfloat)));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void*)(12 * sizeof(GLfloat)));
        glEnableVertexAttribArray(4);
    }
    
    void Mesh::deleteBuffers() {
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
    
    bool Mesh::bufferGenerated() const {
        return _elementsBuffer != 0 && _verticesBuffer != 0 && _indicesBuffer != 0;
    }

}
