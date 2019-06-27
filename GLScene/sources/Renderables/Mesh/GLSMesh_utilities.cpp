//
//  GLSMesh_utilities.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {

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

    void Mesh::sendToFlux(std::ostream& flux, std::string prefixLine) const {
        flux << prefixLine << "[Mesh] {" << std::endl;
        flux << prefixLine << "  vertices: " << _vertices.size() << std::endl;
        flux << prefixLine << "  indices: " << _indices.size() << std::endl;
        flux << prefixLine << "  customShader: " << _shaderProgram << std::endl;
        flux << prefixLine << "  material: " << _material << std::endl;
        flux << prefixLine << "  outlined: " << _outlined << std::endl;
        flux << prefixLine << "}" << std::endl;
    }

}