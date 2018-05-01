//
//  GLSStructs.hpp
//  GLScene
//
//  Created by Arthur Masson on 16/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSStructs_h
#define GLSStructs_h

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/geometric.hpp>// glm::cross, glm::normalize
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace GLS {
    
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
        
        Vertex(glm::vec3 o, glm::vec3 n, glm::vec4 c, glm::vec2 u);
    };
    
}

#endif /* GLSStructs_h */
