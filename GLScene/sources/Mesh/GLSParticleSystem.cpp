//
//  GLSParticleSystem.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSParticleSystem.hpp"

namespace GLS {

    Particle::Particle() :
    position(0),
    velocity(0)
    {

    }

    Particle::Particle(glm::vec3 p, glm::vec3 v) :
    position(p),
    velocity(v)
    {

    }

    const char* ParticleSystem::BufferCreationException::what() const throw() {
        return "can't create additional buffers";
    }

    ParticleSystem::ParticleSystem() :
    _glBuffer(0), _glVertexArray(0)
    {

    }

    ParticleSystem::ParticleSystem(const ParticleSystem& copy) :
    _glBuffer(0), _glVertexArray(0)
    {
        if (copy.bufferGenerated())
            generateBuffers();
    }

    ParticleSystem::~ParticleSystem() {
        deleteBuffers();
    }

    ParticleSystem& ParticleSystem::operator=(const ParticleSystem& copy) {
        if (copy.bufferGenerated())
            generateBuffers();
        return *this;
    }


    // OpenGL Buffers

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

    void ParticleSystem::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
        (void)scene;
        if (!bufferGenerated()) {
            generateBuffers();
            if (!bufferGenerated())
                return;
        }

        std::shared_ptr<ShaderProgram> program;
        program = ShaderProgram::standardShaderProgramDotParticleSystem();
        program->use();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glDisable(GL_STENCIL_TEST);

        uniforms.sendUniformsToShaderProgram(program);

        glUniform3f(program->getLocation("particle_color"), 1, 0, 0);
        
        glPointSize(5);
        glBindVertexArray(_glVertexArray);
        glDrawArrays(GL_POINTS, 0, 1000000);
    }

    std::pair<glm::vec3, glm::vec3> ParticleSystem::getBounds(glm::mat4 transform) const {
        (void)transform;
        return std::make_pair<glm::vec3, glm::vec3>(glm::vec3(0), glm::vec3(0));
    }

    void ParticleSystem::renderInDepthContext(Scene& scene, const RenderUniforms& uniforms) {
        return renderInContext(scene, uniforms);
    }

}
