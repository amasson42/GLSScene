//
//  GLSMesh.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {
    
    const char* Mesh::BufferCreationException::what() const throw() {
        return "can't create additional buffer";
    }

    // Mesh::LoadMeshException::LoadMeshException(std::string filename) : _filename(filename) {}

    // const char* Mesh::LoadMeshException::what() const throw() {
    //     return ("can't load mesh from the file" + _filename).c_str();
    // }

    Mesh::Mesh() : _vertices(), _indices(),
    _verticesBuffer(0), _indicesBuffer(0), _elementsBuffer(0), _bufferGenerated(false),
    _shaderProgram(nullptr),
    _material(nullptr),
    _outlined(false)
    {
        if (_material == nullptr) {
            _material = std::make_shared<Material>();
        }
    }
    
    Mesh::Mesh(const Mesh& copy) :
    _vertices(copy._vertices), _indices(copy._indices),
    _verticesBuffer(0), _indicesBuffer(0), _elementsBuffer(0), _bufferGenerated(false),
    _shaderProgram(nullptr),
    _material(copy._material),
    _outlined(copy._outlined), _outlineColor(copy._outlineColor), _outlineSize(copy._outlineSize)
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
        _material = copy._material;
        if ((_outlined = copy._outlined) != false) {
            _outlineColor = copy._outlineColor;
            _outlineSize = copy._outlineSize;
        }
        return *this;
    }
    
    
    // Mesh utilities
    
    std::vector<Vertex>& Mesh::verticesRef() {
        return _vertices;
    }
    
    std::vector<GLuint>& Mesh::indicesRef() {
        return _indices;
    }
    
    static void calculNormal_normeWith(Vertex& v, glm::vec3 n, int& c) {
        glm::vec3 average = (v.normal * GLfloat(c)) + n;
        average = glm::normalize(average);
        v.normal = average;
        c++;
    }
    
    void Mesh::calculNormals() {
        std::vector<int> coeffs(_vertices.size(), 0);
        Vertex *p[3];
        glm::vec3 v[3];
        for (size_t i = 0; i + 2 < _indices.size(); i += 3) {
            p[0] = &(_vertices[_indices[i + 0]]);
            p[1] = &(_vertices[_indices[i + 1]]);
            p[2] = &(_vertices[_indices[i + 2]]);
            v[0] = p[1]->position - p[0]->position;
            v[1] = p[2]->position - p[0]->position;
            v[2] = glm::cross(v[0], v[1]);
            calculNormal_normeWith(*p[0], v[2], coeffs[_indices[i + 0]]);
            calculNormal_normeWith(*p[1], v[2], coeffs[_indices[i + 1]]);
            calculNormal_normeWith(*p[2], v[2], coeffs[_indices[i + 2]]);
        }
    }
    
    std::pair<glm::vec3, glm::vec3> Mesh::getBounds(glm::mat4 transform) const {
        if (_vertices.empty())
            return std::pair<glm::vec3, glm::vec3>();
        glm::vec3 min = glm::vec3(transform * glm::vec4(_vertices[0].position, 1));
        glm::vec3 max = min;
        for (size_t i = 1; i < _vertices.size(); i++) {
            glm::vec3 p = glm::vec3(transform * glm::vec4(_vertices[i].position, 1));
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
        return std::pair<glm::vec3, glm::vec3>(min, max);
    }

    void Mesh::setMaterial(std::shared_ptr<Material> mat) {
        _material = mat;
    }

    std::shared_ptr<Material> Mesh::getMaterial() const {
        return _material;
    }
    
    void Mesh::setOutline(float size, const glm::vec3& color) {
        _outlined = true;
        _outlineColor = color;
        _outlineSize = 1.0 + size;
    }

    void Mesh::removeOutline() {
        _outlined = false;
    }


    // OpenGL Buffers
    
    void Mesh::generateBuffers() throw(BufferCreationException) {
        if (bufferGenerated())
            deleteBuffers();
        if (_vertices.empty() || _indices.empty())
            return ;

        glGenVertexArrays(1, &_elementsBuffer);
        if (_elementsBuffer == 0)
            throw BufferCreationException();
        glGenBuffers(1, &_verticesBuffer);
        if (_verticesBuffer == 0) {
            glDeleteVertexArrays(1, &_elementsBuffer);
            throw BufferCreationException();
        }
        glGenBuffers(1, &_indicesBuffer);
        if (_indicesBuffer == 0) {
            glDeleteVertexArrays(1, &_elementsBuffer);
            glDeleteBuffers(1, &_verticesBuffer);
            throw BufferCreationException();
        }
        
        glBindVertexArray(_elementsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _verticesBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _vertices.size(),
                     &_vertices.front(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _indices.size(),
                     &_indices.front(), GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void*)(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void*)(9 * sizeof(GLfloat)));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void*)(12 * sizeof(GLfloat)));
        glEnableVertexAttribArray(4);

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
    
    void Mesh::setProgram(std::shared_ptr<ShaderProgram> shaderProgram) {
        _shaderProgram = shaderProgram;
    }

    void Mesh::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
        if (!_bufferGenerated)
            return ;
        
        std::shared_ptr<ShaderProgram> program;
        if (_shaderProgram) {
            program = _shaderProgram;
            program->use();
            scene.sendLightsValueToShader(program);
        } else {
            program = ShaderProgram::standardShaderProgramMesh();
            program->use();
        }
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        glm::mat3 normalMatrix = glm::inverseTranspose(uniforms.model);
        glUniformMatrix4fv(program->getLocation("u_mat_projection"), 1, GL_FALSE, glm::value_ptr(uniforms.projection));
        glUniformMatrix3fv(program->getLocation("u_mat_normal"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniformMatrix4fv(program->getLocation("u_mat_view"), 1, GL_FALSE, glm::value_ptr(uniforms.view));
        glUniformMatrix4fv(program->getLocation("u_mat_model"), 1, GL_FALSE, glm::value_ptr(uniforms.model));
        glUniform3f(program->getLocation("u_camera_position"), uniforms.camera_position.x,
                                                               uniforms.camera_position.y,
                                                               uniforms.camera_position.z);

        if (_outlined) {
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            scene.subscribeToPostRenderable(this, uniforms, 1);
        } else {
            glStencilMask(0x00);
        }

        _material->sendUniformToShaderProgram(program);
        glBindVertexArray(_elementsBuffer);
        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(_indices.size()),
                       GL_UNSIGNED_INT, 0);
        
    }
    
    void Mesh::postRenderInContext(Scene& scene, const RenderUniforms& uniforms, float priority) {
        if (_outlined && priority == 1) {
            // std::cout << "drawing outline of size " << _outlineSize << std::endl;
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramMeshOutline();
            program->use();
            glm::mat4 scaleUpModel = glm::scale(uniforms.model, glm::vec3(_outlineSize));

            glUniformMatrix4fv(program->getLocation("u_mat_projection"), 1, GL_FALSE, glm::value_ptr(uniforms.projection));
            glUniformMatrix4fv(program->getLocation("u_mat_view"), 1, GL_FALSE, glm::value_ptr(uniforms.view));
            glUniformMatrix4fv(program->getLocation("u_mat_model"), 1, GL_FALSE, glm::value_ptr(scaleUpModel));
            glUniform3f(program->getLocation("border_color"), _outlineColor.x, _outlineColor.y, _outlineColor.z);

            glBindVertexArray(_elementsBuffer);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, 0);
            glStencilMask(0xFF);
        }
        (void)scene;
        (void)priority;
    }

}
