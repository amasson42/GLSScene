//
//  GLSMesh.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSMesh_h
#define GLSMesh_h

#include "GLScene.hpp"

namespace GLS {

    class Material {

        public:

        glm::vec3 diffuse;
        glm::vec3 specular;
        GLfloat roughness;
        GLfloat metalness;
        glm::vec3 occlusion;
        GLfloat shininess;

        std::shared_ptr<Texture> texture_diffuse;
        std::shared_ptr<Texture> texture_specular;
        std::shared_ptr<Texture> texture_roughness;
        std::shared_ptr<Texture> texture_metalness;
        std::shared_ptr<Texture> texture_occlusion;
        std::shared_ptr<Texture> texture_shininess;
        std::shared_ptr<Texture> texture_normal;
        std::shared_ptr<Texture> texture_mask;

        Transform2D diffuse_transform;
        Transform2D specular_transform;
        Transform2D roughness_transform;
        Transform2D metalness_transform;
        Transform2D occlusion_transform;
        Transform2D shininess_transform;
        Transform2D normal_transform;
        Transform2D mask_transform;

        Material();
        Material(const Material& copy);
        virtual ~Material();

        Material& operator=(const Material& copy);


        // Utilities

        void sendUniformToShaderProgram(std::shared_ptr<ShaderProgram> program) const;

        static std::shared_ptr<Material> loadFromAiMaterial(aiMaterial *material, const std::string& directory);

    };
    
    class Mesh : public IRenderable {
    
    protected:
        std::vector<Vertex> _vertices;
        std::vector<GLuint> _indices;
        
        GLuint _verticesBuffer;
        GLuint _indicesBuffer;
        GLuint _elementsBuffer;
        
        std::shared_ptr<ShaderProgram> _shaderProgram;
        std::shared_ptr<Material> _material;

        bool _outlined;
        glm::vec3 _outlineColor;
        float _outlineSize;

    public:

        class BufferCreationException : public std::exception {
            public:
            const char* what() const throw();
        };

        class LoadMeshException : public std::exception {
            public:
            const char* what() const throw();
        };

        Mesh();
        Mesh(const Mesh& copy);
        virtual ~Mesh();
        
        Mesh& operator=(const Mesh& copy);
        
        
        // Mesh utilities
        
        std::vector<Vertex>& verticesRef();
        std::vector<GLuint>& indicesRef();
        
        void calculNormals();
        
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

        // Prefabs
        

        static std::shared_ptr<Mesh> plane(GLfloat width, GLfloat height, bool generateBuffers = true);
        static std::shared_ptr<Mesh> cube(GLfloat width, GLfloat height, GLfloat length, bool generateBuffers = true);
        static std::shared_ptr<Mesh> sphere(GLfloat radius, unsigned int ringCount = 12, bool generateBuffers = true);
        static std::shared_ptr<Mesh> objModel(std::string path, bool generateBuffers = true) /*throw(LoadMeshException)*/;
        static std::shared_ptr<Mesh> loadFromAiMesh(aiMesh *mesh, bool generateBuffers = true);

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

        size_t instancesCount() const;
        void setInstancesCount(size_t count);

        const Transform& instanceTransformAt(size_t i) const;
        void setInstanceTransformAt(size_t i, Transform t);

        virtual void generateBuffers() throw(BufferCreationException);
        virtual void deleteBuffers();

        virtual bool bufferGenerated() const;

        void updateTransformsBufferValues();

        virtual void renderInContext(Scene& scene, const RenderUniforms& uniforms);
        virtual void postRenderInContext(Scene& scene, const RenderUniforms& uniforms, float priority);
        virtual void renderInDepthContext(Scene& scene, const RenderUniforms& uniforms);
        
    };
    
}

#endif /* GLSMesh_h */
