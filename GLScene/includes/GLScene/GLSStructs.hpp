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
    class Light;
    class Material;
    class Mesh;
    class Texture;
    class Shader;
    class ShaderProgram;
    
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec3 bitangent;
        glm::vec2 uv;
        
        Vertex();
        
        Vertex(const glm::vec3 p,
               const glm::vec3 n,
               const glm::vec3 t,
               const glm::vec3 b,
               const glm::vec2 u);
        
        Vertex(const glm::vec3& o,
               const glm::vec3& n,
               const glm::vec2& u);
        
    };

    struct DotParticle { // TODO
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec4 color;

        DotParticle();
        
        DotParticle(glm::vec3 p, glm::vec3 v, glm::vec4 c);

    };
    
}

#endif /* GLSStructs_h */
