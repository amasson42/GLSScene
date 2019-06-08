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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        // update values

        const int particleCount = 1000000;
        Particle *particles = new Particle[particleCount];
        for (int i = 0; i < particleCount; i++) {
            glm::vec3 pos(0);
            pos.x = static_cast<float>(random() % 1000) / 500 - 1;
            pos.y = static_cast<float>(random() % 1000) / 500 - 1;
            pos.z = static_cast<float>(random() % 1000) / 500 - 1;
            particles[i] = Particle(pos, glm::vec3(0));
        }

        glBindVertexArray(_glVertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, _glBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * particleCount, particles, GL_STREAM_DRAW);
        delete [] particles;
    }

    void ParticleSystem::deleteBuffers() {
        if (_glBuffer != 0)
            glDeleteBuffers(1, &_glBuffer);
        _glBuffer = 0;
        if (_glVertexArray != 0)
            glDeleteVertexArrays(1, &_glVertexArray);
        _glVertexArray = 0;
    }

    bool ParticleSystem::bufferGenerated() const {
        return _glBuffer != 0 && _glVertexArray != 0;
    }

}
