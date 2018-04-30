//
//  GLSRenderable.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSRenderable_h
#define GLSRenderable_h

#include <string>
#include <vector>
#include "GLScene.hpp"

namespace GLS {
    
    class Renderable {
        
    public:

        virtual void renderInContext(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model) = 0;
        
    };
    
}

#endif /* GLSNode_h */
