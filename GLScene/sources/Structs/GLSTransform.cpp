//
//  GLSTransform.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSTransform.hpp"

namespace GLS {
    
    // Transform 2D

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
        // TODO !
        (void)offset;
        (void)rotation;
        (void)scale;
        return glm::mat3(1);
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


    // Transform 3D

    Transform::Transform() :
    _position(0), _rotation(), _scale(1),
    _transformUpdated(false) {
        updateMatrix();
    }

    Transform::Transform(const Transform& copy) :
    _position(copy._position), _rotation(copy._rotation), _scale(copy._scale),
    _transformUpdated(copy._transformUpdated) {
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


    // Position

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

    void Transform::moveBy(const glm::vec3& offset) {
        _position += offset;
        _transformUpdated = false;
    }

    void Transform::moveBy(GLfloat x, GLfloat y, GLfloat z) {
        _position.x += x;
        _position.y += y;
        _position.z += z;
        _transformUpdated = false;
    }


    // Rotation

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

    void Transform::rotateBy(const glm::quat& rotate) {
        _rotation = rotate * _rotation;
        _transformUpdated = false;
    }

    void Transform::rotateBy(GLfloat x, GLfloat y, GLfloat z, GLfloat alpha) {
        _rotation = glm::quat(alpha, x, y, z) * _rotation;
        _transformUpdated = false;
    }

    const glm::vec3 Transform::eulerAngles() const {
        return glm::eulerAngles(_rotation);
    }

    void Transform::setEulerAngles(const glm::vec3& angles) {
        _rotation = glm::quat(angles);
        _transformUpdated = false;
    }

    void Transform::setEulerAngles(GLfloat x, GLfloat y, GLfloat z) {
        _rotation = glm::quat(glm::vec3(x, y, z));
        _transformUpdated = false;
    }

    void Transform::rotateEulerAnglesBy(GLfloat x, GLfloat y, GLfloat z) {
        glm::vec3 angles = eulerAngles();
        angles.x += x;
        angles.y += y;
        angles.z += z;
        setEulerAngles(angles);
    }



    // Scale

    const glm::vec3& Transform::scale() const {
        return _scale;
    }

    glm::vec3& Transform::scale() {
        _transformUpdated = false;
        return _scale;
    }

    void Transform::setScale(const glm::vec3& scale) {
        _scale = scale;
        _transformUpdated = false;
    }

    void Transform::scaleBy(const glm::vec3& scaler) {
        _scale *= scaler;
        _transformUpdated = false;
    }

    void Transform::scaleBy(GLfloat x, GLfloat y, GLfloat z) {
        _scale.x *= x;
        _scale.y *= y;
        _scale.z *= z;
        _transformUpdated = false;
    }


    // Matrix

    static const glm::mat4 calculTransformMatrix(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
        glm::mat4 mat(1);
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
