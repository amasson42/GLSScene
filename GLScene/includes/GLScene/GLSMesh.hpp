//
//  GLSMesh.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSMesh_h
#define GLSMesh_h

#include "GLSStructs.hpp"
#include "GLSIRenderable.hpp"
#include "GLSTexture.hpp"
#include "GLSMaterial.hpp"

namespace GLS {

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

    class Mesh : public IRenderable {
    
    protected:
        std::vector<Vertex> _vertices;
        std::vector<GLuint> _indices;
        GLenum _drawMode;
        
        GLuint _verticesBuffer;
        GLuint _indicesBuffer;
        GLuint _elementsBuffer;
        
        std::shared_ptr<ShaderProgram> _shaderProgram;
        std::shared_ptr<Material> _material;
        GLuint _castShadowFace;

        bool _outlined;
        glm::vec3 _outlineColor;
        float _outlineSize;

    public:

        Mesh();
        Mesh(const Mesh& copy);
        virtual ~Mesh();
        
        Mesh& operator=(const Mesh& copy);
        
        
        // Mesh utilities
        
        std::vector<Vertex>& verticesRef();
        std::vector<GLuint>& indicesRef();
        void setDrawMode(GLenum mode);
        void setCastShadowFace(GLuint face);
        
        void calculNormals();
        
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


        // Prefabs

        static std::shared_ptr<Mesh> plane(GLfloat width, GLfloat height, bool generateBuffers = true);
        static std::shared_ptr<Mesh> cube(GLfloat width, GLfloat height, GLfloat length, bool generateBuffers = true);
        static std::shared_ptr<Mesh> sphere(GLfloat radius, unsigned int ringCount = 12, bool generateBuffers = true);
        static std::shared_ptr<Mesh> thinLine(glm::vec3 start, glm::vec3 end, bool generateBuffers = true);
        static std::shared_ptr<Mesh> objModel(std::string path, bool generateBuffers = true);
        static std::shared_ptr<Mesh> loadFromAiMesh(aiMesh *mesh, bool generateBuffers = true);


        // Shader uniforms

        static std::string shaderUniformsVertex();
        static std::string shaderUniformsGeometry();
        static std::string shaderUniformsFragment();

    };

    class InstancedMesh : public Mesh {
        protected:

        std::vector<Transform> _instancesTransforms;
        GLuint _transformsBuffer;

        public:

        InstancedMesh();
        InstancedMesh(const Mesh& copy, size_t instanceCount = 1);
        InstancedMesh(const InstancedMesh& copy);
        virtual ~InstancedMesh();

        InstancedMesh& operator=(const Mesh& copy);
        InstancedMesh& operator=(const InstancedMesh& copy);


        // Instances utilities

        size_t instancesCount() const;
        void setInstancesCount(size_t count);

        const Transform& instanceTransformAt(size_t i) const;
        void setInstanceTransformAt(size_t i, Transform t);

        virtual void sendToFlux(std::ostream& flux, std::string linePrefix, std::string firstPrefix, std::string lastPrefix) const;


        // OpenGL Buffers

        virtual void generateBuffers();
        virtual void deleteBuffers();

        virtual bool bufferGenerated() const;

        void updateTransformsBufferValues();


        // Rendering

        virtual void renderInContext(Scene& scene, const RenderUniforms& uniforms);
        virtual void postRenderInContext(Scene& scene, const RenderUniforms& uniforms, float priority);
        virtual void renderInDepthContext(Scene& scene, const RenderUniforms& uniforms);


        // Shader uniforms

        static std::string shaderUniformsVertex();
        static std::string shaderUniformsGeometry();
        static std::string shaderUniformsFragment();
        
    };
    
}

#endif /* GLSMesh_h */
