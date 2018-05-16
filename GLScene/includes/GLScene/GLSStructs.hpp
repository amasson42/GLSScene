//
//  GLSStructs.hpp
//  GLScene
//
//  Created by Arthur Masson on 16/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSStructs_h
#define GLSStructs_h

#include "GLScene/GLSTransform.hpp"

namespace GLS {
    
    class Transform;
    class Scene;
    class Node;
    class Camera;
    class Mesh;
    class Texture;
    class Shader;
    class ShaderProgram;
    
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec4 color;
        glm::vec2 texture;
        
        Vertex();
        Vertex(const glm::vec3& o,
                const glm::vec3& n,
                const glm::vec4& c,
                const glm::vec2& u);
    };
    
}

#endif /* GLSStructs_h */
