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
        static bool first = true;
        if (first) {
            std::cout << "ONE PARTICULE INITIALISED ON RAM !" << std::endl;
            first = false;
        }
    }

    ParticleSystemProperties::ParticleSystemProperties() :
    count(1000000)
    {
        kernelSource = "\n"
        "static float random1f(float x) {\n"
        "    float v = (1.0 + (sin(x) * 2935867.354)) / 2.0;\n"
        "    return (v - floor(v));\n"
        "}\n"
        "\n"
        "kernel void particlesystem_init(global float *buffer, const unsigned int count) {\n"
        "    unsigned int i = get_global_id(0);\n"
        "    if (i < count) {\n"
        "        int p_i = i * 9;\n"

        // position
        "        buffer[p_i + 0] = 0;\n"
        "        buffer[p_i + 1] = 0;\n"
        "        buffer[p_i + 2] = 0;\n"

        "        float velocity = random1f(i + 0.1);\n"
        "        float angleX = 2 * M_PI * random1f(i + random1f(velocity + 4.2));\n"
        "        float angleY = 2 * M_PI * random1f(i + random1f(velocity + 13.37));\n"

        // velocity
        "        buffer[p_i + 3] = velocity * sin(angleY) * cos(angleX);\n"
        "        buffer[p_i + 4] = velocity * sin(angleX);\n"
        "        buffer[p_i + 5] = velocity * cos(angleY) * cos(angleX);\n"

        // color
        "        buffer[p_i + 6] = 0.5;\n"
        "        buffer[p_i + 7] = 0.5;\n"
        "        buffer[p_i + 8] = 0.5;\n"
        "    }\n"
        "    else printf(\"init out... bye !\\n\");\n"
        "}\n"
        "\n"
        "kernel void particlesystem_update(global float *buffer, const unsigned int count, double dt) {\n"
        "    unsigned int i = get_global_id(0);\n"
        "    if (i < count) {\n"
        "        int p_i = i * 9;\n"
        "        buffer[p_i + 0] += buffer[p_i + 3] * dt;\n"
        "        buffer[p_i + 1] += buffer[p_i + 4] * dt;\n"
        "        buffer[p_i + 2] += buffer[p_i + 5] * dt;\n"
        "        buffer[p_i + 6] = cos(buffer[p_i + 0]) * 0.5 + 0.5;\n"
        "        buffer[p_i + 7] = cos(buffer[p_i + 1]) * 0.5 + 0.5;\n"
        "        buffer[p_i + 8] = sin(buffer[p_i + 2]) * 0.5 + 0.5;\n"
        "    }\n"
        "}\n";
    }

    std::shared_ptr<CLD::GPUDevice> ParticleSystem::_device = nullptr;
    int ParticleSystem::_commandQueueIndex = -1;

    void ParticleSystem::_createDevice() {
        _device = std::make_shared<CLD::GPUDevice>();
        std::cout << "ParticuleSystem: Context creation: " << _device->createContext(true) << std::endl;
        _device->createCommandQueue(&_commandQueueIndex);
        std::cout << "ParticuleSystem: queueIndex = " << _commandQueueIndex << std::endl;
    }

    void ParticleSystem::_destroyDevice() {
        _device = nullptr;
    }

    const char* ParticleSystem::BufferCreationException::what() const throw() {
        return "can't create additional buffers";
    }

    ParticleSystem::ParticleSystem(ParticleSystemProperties properties) throw(CLD::GPUDevice::BuildProgramException) :
    _properties(properties),
    _glBuffer(0), _glVertexArray(0),
    _clBufferIndex(-1)
    {
        _createKernels();
    }

    void ParticleSystem::_createKernels() {
        const char *src = _properties.kernelSource.c_str();
        int prgmIndex;

        std::cout << "Creating ParticleSystem with source :" << std::endl << src << std::endl;
        _device->createProgram(&src, &prgmIndex); // will throw if can't compile

        if (prgmIndex < 0)
            throw std::exception(); // TODO: throw can't create program exception

        _device->createKernel(prgmIndex, "particlesystem_init", &_initKernelIndex);
        if (_initKernelIndex < 0) {
            _device->destroyProgram(prgmIndex);
            throw std::exception(); // TODO: throw can't create kernel exception
        }

        _device->createKernel(prgmIndex, "particlesystem_update", &_updateKernelIndex);
        if (_updateKernelIndex < 0) {
            _device->destroyKernel(_initKernelIndex);
            _device->destroyProgram(prgmIndex);
            throw std::exception(); // TODO: throw can't create kernel exception
        }

        _device->destroyProgram(prgmIndex);
    }

    void ParticleSystem::_destroyKernels() {
        _device->destroyKernel(_initKernelIndex);
        _device->destroyKernel(_updateKernelIndex);
    }

    ParticleSystem::ParticleSystem(const ParticleSystem& copy) :
    ParticleSystem(copy._properties)
    {
        if (copy.bufferGenerated())
            generateBuffers();
    }

    ParticleSystem::~ParticleSystem() {
        deleteBuffers();
        _destroyKernels();
    }

    ParticleSystem& ParticleSystem::operator=(const ParticleSystem& copy) {
        deleteBuffers();
        _destroyKernels();
        _properties = copy._properties;
        _createKernels();
        if (copy.bufferGenerated())
            generateBuffers();
        return *this;
    }

    std::pair<glm::vec3, glm::vec3> ParticleSystem::getBounds(glm::mat4 transform) const {
        (void)transform;
        return std::make_pair<glm::vec3, glm::vec3>(glm::vec3(0), glm::vec3(0));
    }

}
