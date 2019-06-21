//
//  GLSParticleSystem_buffers.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSParticleSystem.hpp"

namespace GLS {

    void ParticleSystem::generateBuffers() throw(BufferCreationException) {
        deleteBuffers();

        glGenVertexArrays(1, &_glVertexArray);
        if (_glVertexArray == 0)
            throw BufferCreationException();
        glBindVertexArray(_glVertexArray);

        glGenBuffers(1, &_glBuffer);
        if (_glBuffer == 0) {
            glDeleteVertexArrays(1, &_glVertexArray);
            throw BufferCreationException();
        }
        glBindBuffer(GL_ARRAY_BUFFER, _glBuffer);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void*)(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, _glBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 9 * _properties.count, NULL, GL_STREAM_DRAW);

        CLD::Buffer clBuffer = _device->createGLBuffer(_glBuffer, &_clBufferIndex);
        if (_clBufferIndex < 0)
            throw std::exception(); // TODO: throw can't create buffer exception

        CLD::Kernel *initKernel = _device->kernel(_initKernelIndex);
        initKernel->setArgument(0, clBuffer);
        initKernel->setArgument(1, static_cast<int>(_properties.count));

        CLD::Kernel *updateKernel = _device->kernel(_updateKernelIndex);
        updateKernel->setArgument(0, clBuffer);
        updateKernel->setArgument(1, static_cast<int>(_properties.count));

    }

    void ParticleSystem::deleteBuffers() {
        if (_glBuffer != 0)
            glDeleteBuffers(1, &_glBuffer);
        _glBuffer = 0;
        if (_glVertexArray != 0)
            glDeleteVertexArrays(1, &_glVertexArray);
        _glVertexArray = 0;
        if (_clBufferIndex >= 0)
            _device->destroyBuffer(_clBufferIndex);
        _clBufferIndex = -1;
    }

    bool ParticleSystem::bufferGenerated() const {
        return _glBuffer != 0 && _glVertexArray != 0 && _clBufferIndex >= 0;
    }

}
