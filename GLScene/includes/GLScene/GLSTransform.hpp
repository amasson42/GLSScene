//
//  GLSTransform.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSTransform_h
#define GLSTransform_h

#ifdef __APPLE__
# define GL_SILENCE_DEPRECATION
# include <OpenGL/gl3.h>
#elif defined _WIN32
# include <Windows.h>
# include <glad/glad.h>
# ifndef GLFW_INCLUDE_NONE
#   include <gl/GL.h>
# endif
#endif

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

glm::vec3 aiToGlm(aiVector3D vec);
glm::quat aiToGlm(aiQuaternion quat);
glm::mat4 aiToGlm(aiMatrix4x4 mat);

namespace GLS {

    const float& mat4ValueAt(const glm::mat4& m, int l, int c);
    const float& mat3ValueAt(const glm::mat3& m, int l, int c);
    float& mat4ValueAt(glm::mat4& m, int l, int c);
    float& mat3ValueAt(glm::mat3& m, int l, int c);

    class Transform2D {

        glm::vec2 _offset;
        GLfloat _rotation;
        glm::vec2 _scale;

        glm::mat3 _transform;
        bool _transformUpdated;

    public:

        Transform2D();
        Transform2D(const Transform2D& copy);
        virtual ~Transform2D();

        Transform2D& operator=(const Transform2D& copy);


        // Offset

        const glm::vec2& offset() const;
        glm::vec2& offset();
        void setOffset(const glm::vec2& offset);
        void moveBy(const glm::vec2& offset);
        void moveBy(GLfloat x, GLfloat y);


        // Rotation
        
        const GLfloat& rotation() const;
        GLfloat& rotation();
        void setRotation(GLfloat a);
        void rotateBy(GLfloat a);


        // Scale

        const glm::vec2& scale() const;
        glm::vec2& scale();
        void setScale(const glm::vec2& scale);
        void scaleBy(const glm::vec2& scaler);
        void scaleBy(GLfloat x, GLfloat y);


        // Matrix

        const glm::mat3& matrix();
        const glm::mat3 matrix() const;
        void updateMatrix();

    };

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

        static Transform loadFromAiMatrix(aiMatrix4x4 matrix);

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
        void rotateEulerAnglesBy(GLfloat x, GLfloat y, GLfloat z);


        // Scale

        const glm::vec3& scale() const;
        glm::vec3& scale();
        void setScale(const glm::vec3& scale);
        void scaleBy(const glm::vec3& scaler);
        void scaleBy(GLfloat x, GLfloat y, GLfloat z);


        // Matrix

        void setMatrix(const glm::mat4& mat);
        const glm::mat4& matrix();
        const glm::mat4 matrix() const;
        void updateMatrix();

    };

}

#endif // GLSTransform_h
