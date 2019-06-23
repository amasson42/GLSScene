//
//  GLSParticleSystem.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSParticleSystem.hpp"

namespace GLS {

    Particle::Particle()
    {
        static bool first = true;
        if (first) {
            std::cout << "ONE PARTICULE INITIALISED ON RAM !" << std::endl;
            std::cout << "size = " << sizeof(Particle) << std::endl;
            first = false;
        }
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
    _texture(nullptr),
    _glBuffer(0), _glVertexArray(0),
    _clBufferIndex(-1)
    {
        _createKernels();
    }

    void ParticleSystem::_createKernels() {
        const char *src = _properties.kernelSource.c_str();
        int prgmIndex;

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
        _texture = copy._texture;
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
        _texture = copy._texture;
        if (copy.bufferGenerated())
            generateBuffers();
        return *this;
    }

    void ParticleSystem::setTexture(std::shared_ptr<Texture> texture) {
        _texture = texture;
    }

    std::shared_ptr<Texture> ParticleSystem::getTexture() const {
        return _texture;
    }

    std::pair<glm::vec3, glm::vec3> ParticleSystem::getBounds(glm::mat4 transform) const {
        (void)transform;
        return std::make_pair<glm::vec3, glm::vec3>(glm::vec3(0), glm::vec3(0));
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
        "        int p_i = i * " + std::to_string(sizeof(Particle) / sizeof(float)) + ";\n"
        "        global float3 *position = (global float3*)(buffer + p_i + 0);\n"
        "        global float3 *velocity = (global float3*)(buffer + p_i + 3);\n"
        "        global float3 *color = (global float3*)(buffer + p_i + 6);\n"
        "        global float *size = (global float*)(buffer + p_i + 9);\n"
        "        global float *age = (global float*)(buffer + p_i + 10);\n"

        // position
        "        position->x = 0;\n"
        "        position->y = 0;\n"
        "        position->z = 0;\n"

        "        float magnitude = random1f(i + 0.1);\n"
        "        float angleX = 2 * M_PI * random1f(i + random1f(magnitude + 4.2));\n"
        "        float angleY = 2 * M_PI * random1f(i + random1f(magnitude + 13.37));\n"

        // velocity
        "        velocity->x = magnitude * sin(angleY) * cos(angleX);\n"
        "        velocity->y = magnitude * sin(angleX);\n"
        "        velocity->z = magnitude * cos(angleY) * cos(angleX);\n"

        // color
        "        color->x = 0.5;\n"
        "        color->y = 0.5;\n"
        "        color->z = 0.5;\n"

        // size
        "       *size = 1;\n"

        // age
        "       *age = 3 * ((i / (float)count) - 1);\n"

        "    }\n"
        "    else printf(\"init out... bye !\\n\");\n"
        "}\n"
        "\n"
        "kernel void particlesystem_update(global float *buffer, const unsigned int count, double dt) {\n"
        "    unsigned int i = get_global_id(0);\n"
        "    if (i < count) {\n"
        "        int p_i = i * " + std::to_string(sizeof(Particle) / sizeof(float)) + ";\n"
        "        global float3 *position = (global float3*)(buffer + p_i + 0);\n"
        "        global float3 *velocity = (global float3*)(buffer + p_i + 3);\n"
        "        global float3 *color = (global float3*)(buffer + p_i + 6);\n"
        "        global float *size = (global float*)(buffer + p_i + 9);\n"
        "        global float *age = (global float*)(buffer + p_i + 10);\n"

        "        position->x += velocity->x * (float)dt;\n"
        "        position->y += velocity->y * (float)dt;\n"
        "        position->z += velocity->z * (float)dt;\n"
        "        *age += (float)dt;\n"

        "        color->x = cos(position->x) * 0.5 + 0.5;\n"
        "        color->y = cos(position->y) * 0.5 + 0.5;\n"
        "        color->z = sin(position->z) * 0.5 + 0.5;\n"

        "        *size = (*age + 1);\n"

        "        if (*age >= 3) {\n"
        "            *age -= 3;\n"
        "            position->x = 0;\n"
        "            position->y = 0;\n"
        "            position->z = 0;\n"
        "        }\n"

        "    }\n"
        "}\n";
    }

}
