//
//  GLSDotParticleSystem.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSDotParticleSystem_h
#define GLSDotParticleSystem_h

#include "GLScene.hpp"
#ifdef __APPLE__
# include <OpenCL/OpenCL.h>
#else
# include <OpenCL/cl.h>
#endif

namespace GLS {
    
    class DotParticleSystem : public IRenderable { // TODO
        
    public:
        
        class BufferCreationException : public std::exception {
            public:
            const char* what() const throw();
        };

        DotParticleSystem();
        DotParticleSystem(const DotParticleSystem& copy);
        virtual ~DotParticleSystem();
        
        DotParticleSystem& operator=(const DotParticleSystem& copy);
        
        // Rendering
        
        virtual void renderInContext(Scene& scene, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);
        virtual std::pair<glm::vec3, glm::vec3> getBounds(glm::mat4 transform = glm::mat4(1)) const;

    };
    
}

#endif /* GLSDotParticleSystem_h */
