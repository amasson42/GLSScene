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
    
    class Mesh {
        
        std::vector<Vertex> _vertices;
        std::vector<GLuint> _indices;
        
        GLuint _verticesBuffer;
        GLuint _indicesBuffer;
        GLuint _elementsBuffer;
        bool _bufferGenerated;
        
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
        
        std::pair<Vector, Vector> getBounds() const;
        
        void setColor(Color color);
        
        
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
        
        void renderInContext(ShaderProgram& program, Matrix4x4 view, Matrix4x4 model);
        
        
        // Prefabs
        
        static Mesh plane(GLfloat width, GLfloat height);
        static Mesh cube(GLfloat width, GLfloat height, GLfloat length);
        static Mesh sphere(GLfloat radius, unsigned int ringCount = 12);
        static Mesh objModel(const char *filename, int options);
        static Mesh objModel(std::istream& file, int options);
        
    };
    
}

#endif /* GLSMesh_h */
