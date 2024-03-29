//
//  GLSSkinnedMesh_utilities.cpp
//  GLScene
//
//  Created by Arthur Masson on 26/06/2019.
//  Copyright © 2019 Arthur Masson. All rights reserved.
//

#include "GLSSkinnedMesh.hpp"

namespace GLS {

    std::vector<SkinnedVertex>& SkinnedMesh::verticesRef() {
        return _vertices;
    }

    std::vector<GLuint>& SkinnedMesh::indicesRef() {
        return _indices;
    }

    void SkinnedMesh::setDrawMode(GLenum mode) {
        _drawMode = mode;
    }

    void SkinnedMesh::setSkeleton(std::shared_ptr<Skeleton> skeleton, std::shared_ptr<Node> rootNode) {
        _skeleton = skeleton;
        _rootNode = rootNode;
    }

    const std::shared_ptr<Skeleton> SkinnedMesh::skeleton() const {
        return _skeleton;
    }

    std::pair<glm::vec3, glm::vec3> SkinnedMesh::getBounds(glm::mat4 transform) const {
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

    void SkinnedMesh::setMaterial(std::shared_ptr<Material> mat) {
        _material = mat;
    }

    std::shared_ptr<Material> SkinnedMesh::getMaterial() const {
        return _material;
    }

    void SkinnedMesh::setOutline(float size, const glm::vec3& color) {
        _outlined = true;
        _outlineColor = color;
        _outlineSize = 1.0f + size;
    }

    void SkinnedMesh::removeOutline() {
        _outlined = false;
    }

    void SkinnedMesh::sendToFlux(std::ostream& flux, std::string linePrefix, std::string firstPrefix, std::string lastPrefix) const {
        flux << firstPrefix << "[SkinnedMesh]" << std::endl;
        flux << linePrefix << "  vertices: " << _vertices.size() << std::endl;
        flux << linePrefix << "  indices: " << _indices.size() << std::endl;
        flux << linePrefix << "  customShader: " << _shaderProgram << std::endl;
        flux << linePrefix << "  material: " << _material << std::endl;
        flux << linePrefix << "  outlined: " << _outlined << std::endl;
        flux << linePrefix << "  skeleton: " << _skeleton << std::endl;
        flux << linePrefix << "  rootNode: " << (_rootNode.expired() ? nullptr : _rootNode.lock()) << std::endl;
        flux << lastPrefix << "" << std::endl;
    }

}
