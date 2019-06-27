//
//  GLSSkinnedMesh.hpp
//  GLScene
//
//  Created by Arthur Masson on 26/06/2019.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSSkinnedMesh_h
#define GLSSkinnedMesh_h

#include "GLScene.hpp"

// TODO: make it work lol

namespace GLS {

    struct SkinnedVertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec3 bitangent;
        glm::vec2 uv;
        glm::ivec4 joint_ids;
        glm::vec4 joint_weights;
        
        SkinnedVertex();
        
        SkinnedVertex(const glm::vec3 p,
            const glm::vec3 n,
            const glm::vec3 t,
            const glm::vec3 b,
            const glm::vec2 u);
        
        SkinnedVertex(const glm::vec3& o,
            const glm::vec3& n,
            const glm::vec2& u);

        bool addWeight(int id, float weight);
    };

    class SkinnedMesh : public IRenderable {

    public:
        static const int maxBones = 64;

        struct Bone {
            std::weak_ptr<Node> node;
            glm::mat4 offset;
        };

    protected:
        std::vector<SkinnedVertex> _vertices;
        std::vector<GLuint> _indices;
        
        GLuint _verticesBuffer;
        GLuint _indicesBuffer;
        GLuint _elementsBuffer;

        Bone _rootBone;
        std::vector<Bone> _bones;
        
        std::shared_ptr<ShaderProgram> _shaderProgram;
        std::shared_ptr<Material> _material;

        bool _outlined;
        glm::vec3 _outlineColor;
        float _outlineSize;

        void _sendBonesToShaderProgram(std::shared_ptr<ShaderProgram> program);
    public:

        class BufferCreationException : public std::exception {
            public: const char* what() const throw();
        };

        SkinnedMesh();
        SkinnedMesh(const SkinnedMesh& copy);
        virtual ~SkinnedMesh();
        
        SkinnedMesh& operator=(const SkinnedMesh& copy);

        static std::shared_ptr<SkinnedMesh> loadFromAiMesh(aiMesh* mesh, std::shared_ptr<Node> sceneRootNode, std::shared_ptr<Node> rootBone, bool generateBuffers = true);

        // SkinnedMesh utilities

        std::vector<SkinnedVertex>& verticesRef();
        std::vector<GLuint>& indicesRef();

        std::shared_ptr<Node> rootBone() const;

        virtual std::pair<glm::vec3, glm::vec3> getBounds(glm::mat4 transform = glm::mat4(1)) const;
        
        void setMaterial(std::shared_ptr<Material> mat);
        std::shared_ptr<Material> getMaterial() const;

        void setOutline(float size, const glm::vec3& color);
        void removeOutline();


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

#endif /* GLSSkinnedMesh */
