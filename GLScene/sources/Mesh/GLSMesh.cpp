//
//  GLSMesh.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {
    
    Mesh::Mesh() : _vertices(), _indices(),
    _verticesBuffer(0), _indicesBuffer(0), _elementsBuffer(0), _bufferGenerated(false),
    _diffuse(nullptr), _mask(nullptr), _shininess(0.0f)
    {
        
    }
    
    Mesh::Mesh(const Mesh& copy) :
    _vertices(copy._vertices), _indices(copy._indices),
    _verticesBuffer(0), _indicesBuffer(0), _elementsBuffer(0), _bufferGenerated(false),
    _diffuse(copy._diffuse), _mask(copy._mask), _shininess(copy._shininess)
    {
        if (copy.bufferGenerated())
            generateBuffers();
    }
    
    Mesh::~Mesh() {
        deleteBuffers();
    }
    
    Mesh& Mesh::operator=(const Mesh& copy) {
        _vertices = copy._vertices;
        _indices = copy._indices;
        deleteBuffers();
        if (copy.bufferGenerated())
            generateBuffers();
        setTexture(copy._diffuse);
        setMask(copy._mask);
        _shininess = copy._shininess;
        return *this;
    }
    
    
    // Mesh utilities
    
    std::vector<Vertex>& Mesh::verticesRef() {
        return _vertices;
    }
    
    std::vector<GLuint>& Mesh::indicesRef() {
        return _indices;
    }
    
    static void calculNormal_normeWith(Vertex& v, Vector n, int& c) {
        Vector average = v.getNormal() * c + n;
        average = average.normalized();
        v.setNormal(average);
        c++;
    }
    
    void Mesh::calculNormals() {
        std::vector<int> coeffs(_vertices.size(), 0);
        Vertex *p[3];
        Vector v[3];
        for (size_t i = 0; i + 2 < _indices.size(); i += 3) {
            p[0] = &(_vertices[_indices[i + 0]]);
            p[1] = &(_vertices[_indices[i + 1]]);
            p[2] = &(_vertices[_indices[i + 2]]);
            v[0] = p[1]->getPosition() - p[0]->getPosition();
            v[1] = p[2]->getPosition() - p[0]->getPosition();
            v[2] = Vector::vectorialProduct(v[0], v[1]);
            calculNormal_normeWith(*p[0], v[2], coeffs[_indices[i + 0]]);
            calculNormal_normeWith(*p[1], v[2], coeffs[_indices[i + 1]]);
            calculNormal_normeWith(*p[2], v[2], coeffs[_indices[i + 2]]);
        }
    }
    
    std::pair<Vector, Vector> Mesh::getBounds(Matrix4x4 transform) const {
        if (_vertices.empty())
            return std::pair<Vector, Vector>();
        Vector min;
        Vector max;
        for (size_t i = 0; i < _vertices.size(); i++) {
            Vector p = transform.transform(_vertices[i].getPosition());
            if (p.x < min.x)
                min.x = p.x;
            if (p.y < min.y)
                min.y = p.y;
            if (p.z < min.z)
                min.z = p.z;
            if (p.x > max.x)
                max.x = p.x;
            if (p.y > max.y)
                max.y = p.y;
            if (p.z > max.z)
                max.z = p.z;
        }
        return std::pair<Vector, Vector>(min, max);
    }
    
    void Mesh::setColor(Color color) {
        for (size_t i = 0; i < _vertices.size(); i++)
            _vertices[i].setColor(color);
        if (bufferGenerated())
            generateBuffers();
    }
    
    
    // Material utilities (shall be extern)
    
    void Mesh::setTexture(std::shared_ptr<Texture> texture) {
        _diffuse = texture;
    }
    
    void Mesh::setMask(std::shared_ptr<Texture> mask) {
        _mask = mask;
    }
    
    
    // OpenGL Buffers
    
    void Mesh::generateBuffers() {
        if (bufferGenerated())
            deleteBuffers();
        
        glGenVertexArrays(1, &_elementsBuffer);
        if (_elementsBuffer == 0)
            return ; // throw creating buffer error
        glGenBuffers(1, &_verticesBuffer);
        if (_verticesBuffer == 0) {
            glDeleteVertexArrays(1, &_elementsBuffer);
            return ; // throw creating buffer error
        }
        glGenBuffers(1, &_indicesBuffer);
        if (_indicesBuffer == 0) {
            glDeleteVertexArrays(1, &_elementsBuffer);
            glDeleteBuffers(1, &_verticesBuffer);
            return ; // throw creating buffer error
        }
        
        glBindVertexArray(_elementsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _verticesBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _vertices.size(),
                     &(_vertices[0]), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _indices.size(),
                     &(_indices[0]), GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void*)(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void*)(9 * sizeof(GLfloat)));
        glEnableVertexAttribArray(3);
        
        _bufferGenerated = true;
    }
    
    void Mesh::deleteBuffers() {
        if (bufferGenerated()) {
            if (_elementsBuffer)
                glDeleteVertexArrays(1, &_elementsBuffer);
            if (_verticesBuffer)
                glDeleteBuffers(1, &_verticesBuffer);
            if (_indicesBuffer)
                glDeleteBuffers(1, &_indicesBuffer);
        }
        _verticesBuffer = 0;
        _indicesBuffer = 0;
        _elementsBuffer = 0;
        _bufferGenerated = false;
    }
    
    GLuint Mesh::verticesBuffer() const {
        return _verticesBuffer;
    }
    
    GLuint Mesh::indicesBuffer() const {
        return _indicesBuffer;
    }
    
    GLuint Mesh::elementsBuffer() const {
        return _elementsBuffer;
    }
    
    bool Mesh::bufferGenerated() const {
        return _bufferGenerated;
    }
    
    
    // Rendering
    
    void Mesh::renderInContext(ShaderProgram& program, Matrix4x4 view, Matrix4x4 model) {
        if (!_bufferGenerated)
            return ;
        program.use();
        glUniformMatrix4fv(program.getLocation("view"), 1, GL_FALSE, view.m);
        glUniformMatrix4fv(program.getLocation("model"), 1, GL_FALSE, model.m);
        
        int txbitmask = 0;
        if (_diffuse) {
            txbitmask |= 1 << 1;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _diffuse->buffer());
            glUniform1i(program.getLocation("texture_diffuse"), 0);
        }
        if (_mask) {
            txbitmask |= 1 << 2;
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, _mask->buffer());
            glUniform1i(program.getLocation("texture_mask"), 1);
        }
        txbitmask = txbitmask ? txbitmask : 1;
        
        glUniform1f(program.getLocation("shininess"), _shininess);
        glUniform1i(program.getLocation("texturebitmask"), txbitmask);
        glBindVertexArray(_elementsBuffer);
        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(_indices.size()),
                       GL_UNSIGNED_INT, 0);
    }
    
    
    // Prefabs
    
    // TODO: prefabs
    std::shared_ptr<Mesh> Mesh::plane(GLfloat width, GLfloat height, bool generateBuffers) {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
        width /= 2.0f;
        height /= 2.0f;
        mesh->verticesRef().push_back(Vertex(Vector(-width, -height, 0),
                                             Vector(0, 0, 1),
                                             Color(),
                                             Vector(0, 0)));
        mesh->verticesRef().push_back(Vertex(Vector(-width, height, 0),
                                             Vector(0, 0, 1),
                                             Color(),
                                             Vector(0, 1)));
        mesh->verticesRef().push_back(Vertex(Vector(width, -height, 0),
                                             Vector(0, 0, 1),
                                             Color(),
                                             Vector(1, 0)));
        mesh->verticesRef().push_back(Vertex(Vector(width, height, 0),
                                             Vector(0, 0, 1),
                                             Color(),
                                             Vector(1, 1)));
        const GLuint indices[] = {0, 2, 3, 0, 1, 3}; // reverse last two
        mesh->indicesRef() = std::vector<GLuint>(indices, indices + sizeof(indices) / sizeof(*indices));
        if (generateBuffers)
            mesh->generateBuffers();
        return mesh;
    }
    
//    Mesh Mesh::cube(GLfloat width, GLfloat height, GLfloat length) {
//        
//    }
    
//    Mesh Mesh::sphere(GLfloat radius, unsigned int ringCount = 12);
//    Mesh Mesh::objModel(const char *filename, int options);
//    Mesh Mesh::objModel(std::istream& file, int options);
    
}
