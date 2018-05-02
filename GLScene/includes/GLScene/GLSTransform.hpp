//
//  GLSTransform.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSTransform_h
#define GLSTransform_h

#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/geometric.hpp>// glm::cross, glm::normalize
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace GLS {

    class Transform {

        glm::vec3 _position;
        glm::quat _rotation;
        glm::vec3 _scale;
        glm::mat4 _transform;
        bool _transformUpdated;

    public:

        Transform();
        Transform(const Transform& copy);
        virtual ~Transform();

        Transform& operator=(const Transform& copy);


        // Position

        const glm::vec3& position() const;
        glm::vec3& position();
        void setPosition(const glm::vec3& position);
        void moveBy(const glm::vec3& offset);
        void moveBy(GLfloat x, GLfloat y, GLfloat z);


        // Rotation

        const glm::quat& rotation() const;
        glm::quat& rotation();
        void setRotation(const glm::quat& rotation);
        void rotateBy(const glm::quat& rotate);
        void rotateBy(GLfloat x, GLfloat y, GLfloat z, GLfloat alpha);
        const glm::vec3 eulerAngles() const;
        void setEulerAngles(const glm::vec3& angles);
        void setEulerAngles(GLfloat x, GLfloat y, GLfloat z);


        // Scale

        const glm::vec3& scale() const;
        glm::vec3& scale();
        void setScale(const glm::vec3& scale);
        void scaleBy(const glm::vec3& scaler);
        void scaleBy(GLfloat x, GLfloat y, GLfloat z);


        // Matrix

        const glm::mat4& matrix();
        const glm::mat4 matrix() const;
        void updateMatrix();

    };

}

#endif /* GLSTransform_h */
