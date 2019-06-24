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

std::ostream& operator<<(std::ostream& stream, glm::mat4 m);
std::ostream& operator<<(std::ostream& stream, glm::mat3 m);
std::ostream& operator<<(std::ostream& stream, glm::vec3 v);
std::ostream& operator<<(std::ostream& stream, glm::vec2 v);
std::ostream& operator<<(std::ostream& stream, glm::quat q);

namespace GLS {

    void glsInit();
    void glsDeinit();

    class Transform;
    class Scene;
    class Node;

    struct Camera;
    struct Light;
    struct LightCaster;
    
    class Material;
    class Mesh;
    class InstancedMesh;
    class Skybox;
    class VoxelChunk;
    class ParticleSystem;

    class Texture;
    class Framebuffer;
    class Shader;
    class ShaderProgram;
    
    struct RenderUniforms {
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
        glm::vec3 camera_position;

        RenderUniforms();

        RenderUniforms(const glm::mat4& p,
                       const glm::mat4& v,
                       const glm::mat4& m,
                       const glm::vec3& c);
        
        void sendUniformsToShaderProgram(std::shared_ptr<ShaderProgram> program) const;
    };

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

}

#endif /* GLSStructs_h */
