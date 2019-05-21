//
//  GLSCamera.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSCamera_h
#define GLSCamera_h

#include "GLScene.hpp"

namespace GLS {
    
    struct Camera {
        
        GLfloat fov;
        GLfloat aspect;
        GLfloat nearZ;
        GLfloat farZ;
        
        Camera(GLfloat fov = 1.5708, GLfloat aspect = 1.0f,
               GLfloat nearZ = 0.1f, GLfloat farZ = 100.0f);
        
        glm::mat4 projectionMatrix() const;
        
    };
    
}

#endif /* GLSCamera_h */
