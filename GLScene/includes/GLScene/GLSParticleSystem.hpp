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

namespace GLS {

    struct Particle { // NOTE: never use this structure because it must always be allocated in the vram
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 color;
        float size;
        float age;

        Particle();
    };

    struct ParticleSystemProperties {
        size_t count;
        std::string kernelSource;

        ParticleSystemProperties();
    };

    class ParticleSystem : public IRenderable, public IAnimatable {

        ParticleSystemProperties _properties;
        std::shared_ptr<Texture> _texture;

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
        friend void glsInit(void* addr);
        friend void glsDeinit();

    public:

        ParticleSystem(ParticleSystemProperties properties = ParticleSystemProperties()) throw(CLD::GPUDevice::BuildProgramException);
        ParticleSystem(const ParticleSystem& copy);
        virtual ~ParticleSystem();

        ParticleSystem& operator=(const ParticleSystem& copy);


        void setTexture(std::shared_ptr<Texture> texture);
        std::shared_ptr<Texture> getTexture() const;

        // OpenGL Buffers

        void generateBuffers();
        void deleteBuffers();

        bool bufferGenerated() const;


        // Rendering
        
        virtual void renderInContext(Scene& scene, const RenderUniforms& uniforms);
        virtual std::pair<glm::vec3, glm::vec3> getBounds(glm::mat4 transform = glm::mat4(1)) const;
        virtual void renderInDepthContext(Scene& scene, const RenderUniforms& uniforms);


        // Animating

        CLD::Kernel *getInitKernel();
        CLD::Kernel *getAnimationKernel();
        virtual void initAnimation();
        virtual void animate(float deltaTime);

    };

}

#endif /* GLSParticleSystem_h */
