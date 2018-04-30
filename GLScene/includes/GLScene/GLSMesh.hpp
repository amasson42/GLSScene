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
#include <vector>
#include "GLScene.hpp"

namespace GLS {
    
    class Mesh : public Renderable {
        
        std::vector<Vertex> _vertices;
        std::vector<GLuint> _indices;
        
        GLuint _verticesBuffer;
        GLuint _indicesBuffer;
        GLuint _elementsBuffer;
        bool _bufferGenerated;
        
        std::shared_ptr<ShaderProgram> _shaderProgram;
        
        // TODO: material using here
        std::shared_ptr<Texture> _diffuse;
        std::shared_ptr<Texture> _mask;
        
        float _shininess;
        
    public:
        
        Mesh();
        Mesh(const Mesh& copy);
        virtual ~Mesh();
        
        Mesh& operator=(const Mesh& copy);
        
        
        // Mesh utilities
        
        std::vector<Vertex>& verticesRef();
        std::vector<GLuint>& indicesRef();
        
        void calculNormals();
        
        std::pair<glm::vec3, glm::vec3> getBounds(glm::mat4 transform = glm::mat4(1)) const;
        
        void setColor(glm::vec4 color);
        
        
        // Material utilities (shall be extern)
        
        void setTexture(std::shared_ptr<Texture> texture);
        void setMask(std::shared_ptr<Texture> mask);
        
        
        // OpenGL Buffers
        
        void generateBuffers();
        void deleteBuffers();
        
        GLuint verticesBuffer() const;
        GLuint indicesBuffer() const;
        GLuint elementsBuffer() const;
        bool bufferGenerated() const;
        
        
        // Rendering
        
        void setProgram(std::shared_ptr<ShaderProgram> shaderProgram);

        virtual void renderInContext(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);
        
        
        // Prefabs
        
        static std::shared_ptr<Mesh> plane(GLfloat width, GLfloat height, bool generateBuffers = true);
        static std::shared_ptr<Mesh> cube(GLfloat width, GLfloat height, GLfloat length, bool generateBuffers = true);
        static std::shared_ptr<Mesh> sphere(GLfloat radius, unsigned int ringCount = 12, bool generateBuffers = true);
        static std::shared_ptr<Mesh> objModel(const char *filename, int options, bool generateBuffers = true);
        static std::shared_ptr<Mesh> objModel(std::istream& file, int options, bool generateBuffers = true);
        
    };
    
}

#endif /* GLSMesh_h */
