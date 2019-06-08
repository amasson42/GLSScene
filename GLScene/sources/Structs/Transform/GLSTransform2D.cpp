//
//  GLSTransform2D.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSTransform.hpp"

namespace GLS {

    Transform2D::Transform2D() :
    _offset(0), _rotation(0), _scale(1),
    _transformUpdated(false)
    {
        updateMatrix();
    }

    Transform2D::Transform2D(const Transform2D& copy) :
    _offset(copy._offset), _rotation(copy._rotation), _scale(copy._scale),
    _transformUpdated(copy._transformUpdated) {
        if (_transformUpdated)
            _transform = copy._transform;
    }

    Transform2D::~Transform2D() {

    }

    Transform2D& Transform2D::operator=(const Transform2D& copy) {
        _offset = copy._offset;
        _rotation = copy._rotation;
        _scale = copy._scale;
        if ((_transformUpdated = copy._transformUpdated))
            _transform = copy._transform;
        return *this;
    }

    
    // Offset

    const glm::vec2& Transform2D::offset() const {
        return _offset;
    }

    glm::vec2& Transform2D::offset() {
        _transformUpdated = false;
        return _offset;
    }

    void Transform2D::setOffset(const glm::vec2& offset) {
        _offset = offset;
        _transformUpdated = false;
    }

    void Transform2D::moveBy(const glm::vec2& offset) {
        _offset += offset;
        _transformUpdated = false;
    }

    void Transform2D::moveBy(GLfloat x, GLfloat y) {
        _offset.x += x;
        _offset.y += y;
        _transformUpdated = false;
    }


    // Rotation

    const GLfloat& Transform2D::rotation() const {
        return _rotation;
    }

    GLfloat& Transform2D::rotation() {
        _transformUpdated = false;
        return _rotation;
    }

    void Transform2D::setRotation(GLfloat a) {
        _rotation = a;
        _transformUpdated = false;
    }

    void Transform2D::rotateBy(GLfloat a) {
        _rotation += a;
        _transformUpdated = false;
    }


    // Scale

    const glm::vec2& Transform2D::scale() const {
        return _scale;
    }

    glm::vec2& Transform2D::scale() {
        _transformUpdated = false;
        return _scale;
    }

    void Transform2D::setScale(const glm::vec2& scale) {
        _scale = scale;
        _transformUpdated = false;
    }

    void Transform2D::scaleBy(const glm::vec2& scale) {
        _scale *= scale;
        _transformUpdated = false;
    }

    void Transform2D::scaleBy(GLfloat x, GLfloat y) {
        _scale.x *= x;
        _scale.y *= y;
        _transformUpdated = false;
    }


    // Matrix
    
    static const glm::mat3 calculTransformMatrix(const glm::vec2& offset, const GLfloat rotation, const glm::vec2& scale) {
        glm::mat3 mat;
        float cosa = cos(rotation);
        float sina = sin(rotation);
        mat3ValueAt(mat, 0, 0) = scale.x * cosa;
        mat3ValueAt(mat, 1, 0) = scale.x * sina;
        mat3ValueAt(mat, 2, 0) = 0;
        mat3ValueAt(mat, 0, 1) = scale.y * -sina;
        mat3ValueAt(mat, 1, 1) = scale.y * cosa;
        mat3ValueAt(mat, 2, 1) = 0;
        mat3ValueAt(mat, 0, 2) = offset.x;
        mat3ValueAt(mat, 1, 2) = offset.y;
        mat3ValueAt(mat, 2, 2) = 1;
        return mat;
    }

    const glm::mat3& Transform2D::matrix() {
        if (!_transformUpdated)
            updateMatrix();
        return _transform;
    }

    const glm::mat3 Transform2D::matrix() const {
        if (_transformUpdated)
            return _transform;
        else
            return calculTransformMatrix(_offset, _rotation, _scale);
    }

    void Transform2D::updateMatrix() {
        if (!_transformUpdated) {
            _transform = calculTransformMatrix(_offset, _rotation, _scale);
            _transformUpdated = true;
        }
    }

}
