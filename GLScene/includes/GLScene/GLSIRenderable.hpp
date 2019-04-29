//
//  GLSRenderable.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSRenderable_h
#define GLSRenderable_h

#include "GLScene.hpp"

namespace GLS {
    
    class IRenderable {
        
    public:

        virtual void renderInContext(Scene& scene, const RenderUniforms& uniforms) = 0;
        virtual void postRenderInContext(Scene& scene, const RenderUniforms& uniforms, float priority) {(void)scene;(void)uniforms;(void)priority;}
        virtual std::pair<glm::vec3, glm::vec3> getBounds(glm::mat4 transform = glm::mat4(1)) const = 0;
        
    };
    
}

#endif /* GLSNode_h */
