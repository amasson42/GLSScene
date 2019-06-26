//
//  GLSAnimatedModel.hpp
//  GLScene
//
//  Created by Arthur Masson on 26/06/2019.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSAnimatedModel_h
#define GLSAnimatedModel_h

#include "GLScene.hpp"

// TODO: make it work lol

namespace GLS {

    // std::cout << (size_t)&(vertex) << std::endl;
    // std::cout << (size_t)&(vertex.position)       << ' ' << (size_t)(&(vertex)) + sizeof(float) * 0 + sizeof(int) * 0 << std::endl;
    // std::cout << (size_t)&(vertex.normal)         << ' ' << (size_t)(&(vertex)) + sizeof(float) * 3 + sizeof(int) * 0 << std::endl;
    // std::cout << (size_t)&(vertex.tangent)        << ' ' << (size_t)(&(vertex)) + sizeof(float) * 6 + sizeof(int) * 0 << std::endl;
    // std::cout << (size_t)&(vertex.bitangent)      << ' ' << (size_t)(&(vertex)) + sizeof(float) * 9 + sizeof(int) * 0 << std::endl;
    // std::cout << (size_t)&(vertex.uv)             << ' ' << (size_t)(&(vertex)) + sizeof(float) * 12 + sizeof(int) * 0 << std::endl;
    // std::cout << (size_t)&(vertex.joint_ids)      << ' ' << (size_t)(&(vertex)) + sizeof(float) * 14 + sizeof(int) * 0 << std::endl;
    // std::cout << (size_t)&(vertex.joint_weights)  << ' ' << (size_t)(&(vertex)) + sizeof(float) * 14 + sizeof(int) * 4 << std::endl;

    struct AnimatedVertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec3 bitangent;
        glm::vec2 uv;
        glm::ivec4 joint_ids;
        glm::vec4 joint_weights;
        
        AnimatedVertex();
        
        AnimatedVertex(const glm::vec3 p,
            const glm::vec3 n,
            const glm::vec3 t,
            const glm::vec3 b,
            const glm::vec2 u);
        
        AnimatedVertex(const glm::vec3& o,
            const glm::vec3& n,
            const glm::vec2& u);
    };

    struct Joint {
        int id;
        std::string name;
        std::vector<int> childs;
        Transform transform;
    };

    struct JointTree {
        static const int max_joints = 16;
        Joint joints[max_joints];

        void sendUniformsToShaderProgram(std::shared_ptr<ShaderProgram> program) const;
    };

    class AnimatedModel : public IRenderable {

        std::shared_ptr<ShaderProgram> _shaderProgram;
        std::shared_ptr<Material> _material;

    public:

        class BufferCreationException : public std::exception {
            public: const char* what() const throw();
        };

        AnimatedModel();
        AnimatedModel(const AnimatedModel& copy);
        virtual ~AnimatedModel();
        
        AnimatedModel& operator=(const AnimatedModel& copy);


        // AnimatedModel utilities
        
        virtual std::pair<glm::vec3, glm::vec3> getBounds(glm::mat4 transform = glm::mat4(1)) const;
        
        void setMaterial(std::shared_ptr<Material> mat);
        std::shared_ptr<Material> getMaterial() const;


        // OpenGL Buffers
         
        virtual void generateBuffers() throw(BufferCreationException);
        virtual void deleteBuffers();
        
        virtual bool bufferGenerated() const;
        
        
        // Rendering
        
        void setProgram(std::shared_ptr<ShaderProgram> shaderProgram);
        
        virtual void renderInContext(Scene& scene, const RenderUniforms& uniforms);
        virtual void postRenderInContext(Scene& scene, const RenderUniforms& uniforms, float priority);
        virtual void renderInDepthContext(Scene& scene, const RenderUniforms& uniforms);


        // Shader uniforms

        static std::string shaderUniformsVertex();
        static std::string shaderUniformsGeometry();
        static std::string shaderUniformsFragment();

    };

}

#endif /* GLSAnimatedModel */
