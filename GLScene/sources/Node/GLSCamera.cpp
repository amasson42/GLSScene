//
//  GLSCamera.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSCamera.hpp"

namespace GLS {
    
    Camera::Camera(GLfloat fov, GLfloat aspect, GLfloat nearZ, GLfloat farZ) : 
    _fov(fov), _aspect(aspect), _nearZ(nearZ), _farZ(farZ) {
        updateProjectionMatrix();
    }
    
    Camera::Camera(const Camera& copy) :
    _fov(copy._fov), _aspect(copy._aspect), _nearZ(copy._nearZ), _farZ(copy._farZ) {
        if (copy._projectionUpdated)
            _projection = copy._projection;
        _projectionUpdated = copy._projectionUpdated;
    }
    
    Camera::~Camera() {
        
    }
    
    Camera& Camera::operator=(const Camera& copy) {
        _fov = copy._fov;
        _aspect = copy._aspect;
        _nearZ = copy._nearZ;
        _farZ = copy._farZ;
        if (copy._projectionUpdated)
            _projection = copy._projection;
        _projectionUpdated = copy._projectionUpdated;
        return *this;
    }
    
    GLfloat Camera::fov() const {
        return _fov;
    }
    
    void Camera::setFov(GLfloat fov) {
        _fov = fov;
        _projectionUpdated = false;
    }
    
    GLfloat Camera::aspect() const {
        return _aspect;
    }
    
    void Camera::setAspect(GLfloat aspect) {
        _aspect = aspect;
        _projectionUpdated = false;
    }
    
    GLfloat Camera::nearZ() const {
        return _nearZ;
    }
    
    void Camera::setNearZ(GLfloat nearZ) {
        _nearZ = nearZ;
        _projectionUpdated = false;
    }
    
    GLfloat Camera::farZ() const {
        return _farZ;
    }
    
    void Camera::setFarZ(GLfloat farZ) {
        _farZ = farZ;
        _projectionUpdated = false;
    }
    
    glm::mat4 Camera::projectionMatrix() const {
        if (_projectionUpdated)
            return _projection;
        else
            return glm::perspective(_fov, _aspect, _nearZ, _farZ);
    }
    
    glm::mat4 Camera::projectionMatrix() {
        if (!_projectionUpdated)
            updateProjectionMatrix();
        return _projection;
    }
    
    void Camera::updateProjectionMatrix() {
        _projection = glm::perspective(_fov, _aspect, _nearZ, _farZ);
        _projectionUpdated = true;
    }
    
}
