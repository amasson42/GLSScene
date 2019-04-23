//
//  GLSStructs.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLScene.hpp"

namespace GLS {
    
    Vertex::Vertex() {
        position = glm::vec3(0);
        normal = glm::vec3(0, 0, -1);
        tangent = glm::vec3(1, 0, 0);
        bitangent = glm::vec3(0, 1, 0);
        uv = glm::vec2(0);
    }

    Vertex::Vertex(const glm::vec3 p,
               const glm::vec3 n,
               const glm::vec3 t,
               const glm::vec3 b,
               const glm::vec2 u) {
        position = p;
        normal = n;
        tangent = t;
        bitangent = b;
        uv = u;
    }
    
    Vertex::Vertex(const glm::vec3& o,
           const glm::vec3& n,
           const glm::vec2& u) {
        position = o;
        normal = n;
        if (n == glm::vec3(0, 1, 0))
            tangent = glm::vec3(1, 0, 0);
        else
            tangent = glm::cross(glm::vec3(0, 1, 0), n);
        bitangent = glm::cross(n, tangent);
        uv = u;
    }

}
