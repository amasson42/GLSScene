//
//  GLSCamera.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSCamera.hpp"

namespace GLS {
    
    Camera::Camera(GLfloat f, GLfloat a, GLfloat nZ, GLfloat fZ) : 
    fov(f), aspect(a), nearZ(nZ), farZ(fZ),
    fogNear(0.8 * fZ), fogFar(fZ) {
        
    }
    
    glm::mat4 Camera::projectionMatrix() const {
        return glm::perspective(fov, aspect, nearZ, farZ);
    }
    
}
