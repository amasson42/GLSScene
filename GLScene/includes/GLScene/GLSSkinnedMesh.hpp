//
//  GLSSkinnedMesh.hpp
//  GLScene
//
//  Created by Arthur Masson on 26/06/2019.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSSkinnedMesh_h
#define GLSSkinnedMesh_h

#include "GLSStructs.hpp"
#include "GLSNode.hpp"
#include "GLSIRenderable.hpp"
#include "GLSIAnimatable.hpp"
#include "GLSMaterial.hpp"
#include "GLSSkeleton.hpp"

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

    protected:
        std::vector<SkinnedVertex> _vertices;
        std::vector<GLuint> _indices;
        GLenum _drawMode;
        
        GLuint _verticesBuffer;
        GLuint _indicesBuffer;
        GLuint _elementsBuffer;

        std::shared_ptr<Skeleton> _skeleton;
        std::weak_ptr<Node> _rootNode;
        
        std::shared_ptr<ShaderProgram> _shaderProgram;
        std::shared_ptr<Material> _material;

        bool _outlined;
        glm::vec3 _outlineColor;
        float _outlineSize;

        void _sendBonesToShaderProgram(std::shared_ptr<ShaderProgram> program);
    public:

        SkinnedMesh();
        SkinnedMesh(const SkinnedMesh& copy);
        virtual ~SkinnedMesh();
        
        SkinnedMesh& operator=(const SkinnedMesh& copy);

        static std::shared_ptr<SkinnedMesh> loadFromAiMesh(const aiMesh* mesh, std::shared_ptr<Skeleton> skeleton, std::shared_ptr<Node> rootNode, bool generateBuffers = true);

        // SkinnedMesh utilities

        std::vector<SkinnedVertex>& verticesRef();
        std::vector<GLuint>& indicesRef();
        void setDrawMode(GLenum mode);

        const std::shared_ptr<Skeleton> skeleton() const;

        virtual std::pair<glm::vec3, glm::vec3> getBounds(glm::mat4 transform = glm::mat4(1)) const;
        
        void setMaterial(std::shared_ptr<Material> mat);
        std::shared_ptr<Material> getMaterial() const;

        void setOutline(float size, const glm::vec3& color);
        void removeOutline();

        virtual void sendToFlux(std::ostream& flux, std::string linePrefix, std::string firstPrefix, std::string lastPrefix) const;


        // OpenGL Buffers
        
        virtual void generateBuffers();
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
