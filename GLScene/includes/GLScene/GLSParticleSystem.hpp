//
//  GLSParticleSystem.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSParticleSystem_h
#define GLSParticleSystem_h

#include "GLScene.hpp"
#ifdef __APPLE__
# include <OpenCL/OpenCL.h>
#else
# include <OpenCL/cl.h>
#endif

namespace GLS {

    struct Particle { // NOTE: never use this structure because it must always be allocated in the vram
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 color;

        Particle();
    };

    struct ParticleSystemProperties {
        size_t count;
        std::string kernelSource;

        ParticleSystemProperties();
    };

    class ParticleSystem : public IRenderable {

        ParticleSystemProperties _properties;

        GLuint _glBuffer;
        GLuint _glVertexArray;

        int _clBufferIndex;
        int _initKernelIndex;
        int _updateKernelIndex;
        void _createKernels();
        void _destroyKernels();

        static std::shared_ptr<CLD::GPUDevice> _device;
        static int _commandQueueIndex;
        static void _createDevice();
        static void _destroyDevice();
        friend void glsInit();
        friend void glsDeinit();

    public:
        
        class BufferCreationException : public std::exception {
            public:
            const char* what() const throw();
        };

        ParticleSystem(ParticleSystemProperties properties = ParticleSystemProperties()) throw(CLD::GPUDevice::BuildProgramException);
        ParticleSystem(const ParticleSystem& copy);
        virtual ~ParticleSystem();

        ParticleSystem& operator=(const ParticleSystem& copy);


        // OpenGL Buffers

        void generateBuffers() throw(BufferCreationException);
        void deleteBuffers();

        bool bufferGenerated() const;


        // Rendering
        
        virtual void renderInContext(Scene& scene, const RenderUniforms& uniforms);
        virtual std::pair<glm::vec3, glm::vec3> getBounds(glm::mat4 transform = glm::mat4(1)) const;
        virtual void renderInDepthContext(Scene& scene, const RenderUniforms& uniforms);


        // Animating

        CLD::Kernel *getAnimationKernel();
        virtual void initAnimation();
        virtual void animateWithDeltaTime(double deltaTime);

    };

}

#endif /* GLSParticleSystem_h */
