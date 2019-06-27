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

    std::ostream& operator<<(std::ostream& stream, const glm::mat4& m);
    std::ostream& operator<<(std::ostream& stream, const glm::mat3& m);
    std::ostream& operator<<(std::ostream& stream, const glm::vec3& v);
    std::ostream& operator<<(std::ostream& stream, const glm::vec2& v);
    std::ostream& operator<<(std::ostream& stream, const glm::quat& q);

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
    class SkinnedMesh;
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

}

#endif /* GLSStructs_h */
