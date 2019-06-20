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
        CLD::GPUDevice device;
        std::cout << device << std::endl;
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

    std::pair<glm::vec3, glm::vec3> ParticleSystem::getBounds(glm::mat4 transform) const {
        (void)transform;
        return std::make_pair<glm::vec3, glm::vec3>(glm::vec3(0), glm::vec3(0));
    }

}
