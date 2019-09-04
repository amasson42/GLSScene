//
//  GLSCamera.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSCamera_h
#define GLSCamera_h

#include "GLSStructs.hpp"

namespace GLS {
    
    struct Camera {
        
        float fov;
        float aspect;
        float nearZ;
        float farZ;
        
        float fogNear;
        float fogFar;
        
        Camera(float fov = 1.5708, float aspect = 1.0f,
               float nearZ = 0.1f, float farZ = 100.0f);
        
        glm::mat4 projectionMatrix() const;
        
    };
    
}

#endif /* GLSCamera_h */
