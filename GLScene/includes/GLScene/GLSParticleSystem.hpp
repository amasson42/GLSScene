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

    struct Particle { // TODO: never use this structure because it must always be allocated in the vram
        glm::vec3 position;
        glm::vec3 velocity;
        // TODO:
        // glm::vec3 color;
        // float size;
        // float lifeTime;

        Particle();

        Particle(glm::vec3 p, glm::vec3 v);

    };

    class ParticleSystem : public IRenderable {
        // TODO: Manage update of particles with openCL
        // TODO: allow the updating openCL compute shader to be customized
        
        // static CLS::GPUDevice _device; //TODO: manage creation and destruction
        GLuint _glBuffer;
        GLuint _glVertexArray;

    public:
        
        class BufferCreationException : public std::exception {
            public:
            const char* what() const throw();
        };

        ParticleSystem();
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

    };

}

#endif /* GLSParticleSystem_h */
