//
//  GLSMesh.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {

    Mesh::Mesh() :
    IRenderable(),
    _vertices(), _indices(), _drawMode(GL_TRIANGLES),
    _verticesBuffer(0), _indicesBuffer(0), _elementsBuffer(0),
    _shaderProgram(nullptr),
    _material(nullptr),
    _castShadowFace(GL_FRONT),
    _outlined(false)
    {

    }

    Mesh::Mesh(const Mesh& copy) :
    IRenderable(copy),
    _vertices(copy._vertices), _indices(copy._indices), _drawMode(copy._drawMode),
    _verticesBuffer(0), _indicesBuffer(0), _elementsBuffer(0),
    _shaderProgram(copy._shaderProgram),
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
        IRenderable::operator=(copy);
        _vertices = copy._vertices;
        _indices = copy._indices;
        _drawMode = copy._drawMode;
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
