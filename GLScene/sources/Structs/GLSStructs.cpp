//
//  GLSStructs.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLScene.hpp"

namespace GLS {
    
    Vertex::Vertex(const glm::vec3& o,
                    const glm::vec3& n,
                    const glm::vec4& c,
                    const glm::vec2& u) {
        position = o;
        normal = n;
        color = c;
        texture = u;
    }
    
}
