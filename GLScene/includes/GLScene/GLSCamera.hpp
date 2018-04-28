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
    
    class Camera {
        
        GLfloat _fov;
        GLfloat _aspect;
        GLfloat _nearZ;
        GLfloat _farZ;
        
        glm::mat4 _projection;
        bool _projectionUpdated;
        
    public:
        
        Camera(GLfloat fov = 60.0f, GLfloat aspect = 1.0f,
               GLfloat nearZ = 0.1f, GLfloat farZ = 100.0f);
        Camera(const Camera& copy);
        virtual ~Camera();
        
        Camera& operator=(const Camera& copy);
        
        GLfloat fov() const;
        void setFov(GLfloat fov);
        
        GLfloat aspect() const;
        void setAspect(GLfloat aspect);
        
        GLfloat nearZ() const;
        void setNearZ(GLfloat nearZ);
        
        GLfloat farZ() const;
        void setFarZ(GLfloat farZ);
        
        glm::mat4 projectionMatrix() const;
        glm::mat4 projectionMatrix();
        void updateProjectionMatrix();
        
    };
    
}

#endif /* GLSCamera_h */
