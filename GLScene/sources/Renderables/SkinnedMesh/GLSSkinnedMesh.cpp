//
//  GLSSkinnedMesh.cpp
//  GLScene
//
//  Created by Arthur Masson on 26/06/2019.
//  Copyright © 2019 Arthur Masson. All rights reserved.
//

#include "GLSSkinnedMesh.hpp"

namespace GLS {

    SkinnedMesh::SkinnedMesh() :
    _vertices(), _indices(), _drawMode(GL_TRIANGLES),
    _verticesBuffer(0), _indicesBuffer(0), _elementsBuffer(0),
    _rootBone(), _bones(),
    _shaderProgram(nullptr),
    _material(nullptr),
    _outlined(false)
    {

    }

    SkinnedMesh::SkinnedMesh(const SkinnedMesh& copy) :
    _vertices(copy._vertices), _indices(copy._indices), _drawMode(copy._drawMode),
    _verticesBuffer(0), _indicesBuffer(0), _elementsBuffer(0),
    _rootBone(copy._rootBone), _bones(copy._bones),
    _shaderProgram(copy._shaderProgram),
    _material(copy._material),
    _outlined(copy._outlined), _outlineColor(copy._outlineColor), _outlineSize(copy._outlineSize)
    {
        if (copy.bufferGenerated())
            generateBuffers();
    }

    SkinnedMesh::~SkinnedMesh() {
        deleteBuffers();
    }

    SkinnedMesh& SkinnedMesh::operator=(const SkinnedMesh& copy) {
        _vertices = copy._vertices;
        _indices = copy._indices;
        _drawMode = copy._drawMode;
        _rootBone = copy._rootBone;
        _shaderProgram = copy._shaderProgram;
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

}
