//
//  GLSMesh.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSMesh_h
#define GLSMesh_h

#include <fstream>
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

        // TODO: read info from file

    };
    
    class Mesh : public IRenderable {
        
        std::vector<Vertex> _vertices;
        std::vector<GLuint> _indices;
        
        GLuint _verticesBuffer;
        GLuint _indicesBuffer;
        GLuint _elementsBuffer;
        bool _bufferGenerated;
        
        std::shared_ptr<ShaderProgram> _shaderProgram;
        std::shared_ptr<Material> _material;
        
    public:
        
        class BufferCreationException : public std::exception {
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
        
        // OpenGL Buffers
         
        void generateBuffers() throw(BufferCreationException);
        void deleteBuffers();
        
        GLuint verticesBuffer() const;
        GLuint indicesBuffer() const;
        GLuint elementsBuffer() const;
        bool bufferGenerated() const;
        
        
        // Rendering
        
        void setProgram(std::shared_ptr<ShaderProgram> shaderProgram);
        
        virtual void renderInContext(Scene& scene, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);
        
        
        // Prefabs
        
        static std::shared_ptr<Mesh> plane(GLfloat width, GLfloat height, bool generateBuffers = true);
        static std::shared_ptr<Mesh> cube(GLfloat width, GLfloat height, GLfloat length, bool generateBuffers = true);
        static std::shared_ptr<Mesh> sphere(GLfloat radius, unsigned int ringCount = 12, bool generateBuffers = true);
        static std::shared_ptr<Mesh> objModel(const char *filename, int options, bool generateBuffers = true);
        static std::shared_ptr<Mesh> objModel(std::istream& file, int options, bool generateBuffers = true);
        
    };
    

}

#endif /* GLSMesh_h */
