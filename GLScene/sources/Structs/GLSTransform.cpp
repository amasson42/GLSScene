//
//  GLSTransform.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSTransform.hpp"

namespace GLS {
    
    Transform::Transform() :
    _position(0), _rotation(), _scale(1),
    _transformUpdated(false) {
        updateMatrix();
    }

    Transform::Transform(const Transform& copy) :
    _position(copy._position), _rotation(copy._rotation), _scale(copy._scale),
    _transformUpdated(false) {
        if (_transformUpdated)
            _transform = copy._transform;
    }

    Transform::~Transform() {

    }

    Transform& Transform::operator=(const Transform& copy) {
        _position = copy._position;
        _rotation = copy._rotation;
        _scale = copy._scale;
        if ((_transformUpdated = copy._transformUpdated))
            _transform = copy._transform;
        return *this;
    }

    const glm::vec3& Transform::position() const {
        return _position;
    }

    glm::vec3& Transform::position() {
        _transformUpdated = false;
        return _position;
    }

    void Transform::setPosition(const glm::vec3& position) {
        _position = position;
        _transformUpdated = false;
    }
    
    const glm::quat& Transform::rotation() const {
        return _rotation;
    }

    glm::quat& Transform::rotation() {
        _transformUpdated = false;
        return _rotation;
    }

    void Transform::setRotation(const glm::quat& rotation) {
        _rotation = rotation;
        _transformUpdated = false;
    }
    
    const glm::vec3& Transform::scale() const {
        return _scale;
    }

    glm::vec3& Transform::scale() {
        _transformUpdated = false;
        return _scale;
    }

    void Transform::setScale(const glm::vec3& scale) {
        _scale = scale;
    }

    static const glm::mat4 calculTransformMatrix(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
        glm::mat4 mat;
        mat = glm::translate(mat, position);
        mat = mat * glm::toMat4(rotation);
        mat = glm::scale(mat, scale);
        return mat;
    }

    const glm::mat4& Transform::matrix() {
        if (!_transformUpdated)
            updateMatrix();
        return _transform;
    }

    const glm::mat4 Transform::matrix() const {
        if (_transformUpdated)
            return _transform;
        else
            return calculTransformMatrix(_position, _rotation, _scale);
    }

    void Transform::updateMatrix() {
        if (!_transformUpdated) {
            _transform = calculTransformMatrix(_position, _rotation, _scale);
            _transformUpdated = true;
        }
    }
    
}
