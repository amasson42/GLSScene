//
//  GLSInstancedMesh.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {

    InstancedMesh::InstancedMesh() :
    Mesh(),
    _instancesTransforms(1, Transform()),
    _transformsBuffer(0)
    {

    }

    InstancedMesh::InstancedMesh(const Mesh& copy, size_t instanceCount) :
    Mesh(copy),
    _instancesTransforms(instanceCount, Transform()),
    _transformsBuffer(0)
    {

    }

    InstancedMesh::InstancedMesh(const InstancedMesh& copy) :
    Mesh(copy),
    _instancesTransforms(copy._instancesTransforms),
    _transformsBuffer(0)
    {
        if (copy.bufferGenerated())
            generateBuffers();
    }

    InstancedMesh::~InstancedMesh() {

    }

    InstancedMesh& InstancedMesh::operator=(const Mesh& copy) {
        _instancesTransforms.clear();
        _instancesTransforms.push_back(Transform());
        Mesh::operator=(copy);
        return *this;
    }

    InstancedMesh& InstancedMesh::operator=(const InstancedMesh& copy) {
        _instancesTransforms = copy._instancesTransforms;
        Mesh::operator=(copy);
        return *this;
    }

    size_t InstancedMesh::instancesCount() const {
        return _instancesTransforms.size();
    }

    void InstancedMesh::setInstancesCount(size_t count) {
        _instancesTransforms.resize(count, Transform());
        deleteBuffers();
    }

    const Transform& InstancedMesh::instanceTransformAt(size_t i) const {
        return _instancesTransforms[i];
    }

    void InstancedMesh::setInstanceTransformAt(size_t i, Transform t) {
        _instancesTransforms[i] = t;
    }

    void InstancedMesh::sendToFlux(std::ostream& flux, std::string linePrefix, std::string firstPrefix, std::string lastPrefix) const {
        flux << firstPrefix << "[InstancedMesh]" << std::endl;
        flux << linePrefix << "  vertices: " << _vertices.size() << std::endl;
        flux << linePrefix << "  indices: " << _indices.size() << std::endl;
        flux << linePrefix << "  customShader: " << _shaderProgram << std::endl;
        flux << linePrefix << "  material: " << _material << std::endl;
        flux << linePrefix << "  outlined: " << _outlined << std::endl;
        flux << linePrefix << "  instances: " << _instancesTransforms.size() << std::endl;
        if (_instancesTransforms.size() < 42) {
            for (size_t i = 0; i < _instancesTransforms.size(); i++) {
                flux << linePrefix << "    " << _instancesTransforms[i] << std::endl;
            }
        }
        flux << lastPrefix << "" << std::endl;
    }

}
