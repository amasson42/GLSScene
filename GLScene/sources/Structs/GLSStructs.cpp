//
//  GLSStructs.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSStructs.hpp"


std::ostream& operator<<(std::ostream& stream, const glm::mat4& m) {
    for (int i = 0; i < 16; i++) {
        stream << GLS::mat4ValueAt(m, i / 4, i % 4) << ((i + 1) % 4 ? ' ' : '\n');
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const glm::mat3& m) {
    for (int i = 0; i < 9; i++) {
        stream << GLS::mat3ValueAt(m, i / 3, i % 3) << ((i + 1) % 3 ? ' ' : '\n');
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const glm::vec2& v) {
    stream << '(' << v.x << ',' << v.y << ')';
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const glm::vec3& v) {
    stream << '(' << v.x << ',' << v.y << ',' << v.z << ')';
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const glm::vec4& v) {
    stream << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ')';
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const glm::ivec3& v) {
    stream << '(' << v.x << ',' << v.y << ',' << v.z << ')';
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const glm::ivec2& v) {
    stream << '(' << v.x << ',' << v.y << ')';
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const glm::quat& q) {
    stream << '(' << q.x << ',' << q.y << ',' << q.z << ',' << q.w << ')';
    return stream;
}

namespace GLS {

    glm::vec3 aiToGlm(aiVector3D vec) {
        return glm::vec3(vec.x, vec.y, vec.z);
    }

    glm::quat aiToGlm(aiQuaternion quat) {
        return glm::quat(quat.w, quat.x, quat.y, quat.z);
    }

    glm::mat4 aiToGlm(aiMatrix4x4 aiMat) {
        glm::mat4 mat;
        glm::f32 *ptr = glm::value_ptr(mat);

        ptr[0] =    (aiMat.a1);
        ptr[1] =    (aiMat.b1);
        ptr[2] =    (aiMat.c1);
        ptr[3] =    (aiMat.d1);

        ptr[4] =    (aiMat.a2);
        ptr[5] =    (aiMat.b2);
        ptr[6] =    (aiMat.c2);
        ptr[7] =    (aiMat.d2);

        ptr[8] =    (aiMat.a3);
        ptr[9] =    (aiMat.b3);
        ptr[10] =   (aiMat.c3);
        ptr[11] =   (aiMat.d3);

        ptr[12] =   (aiMat.a4);
        ptr[13] =   (aiMat.b4);
        ptr[14] =   (aiMat.c4);
        ptr[15] =   (aiMat.d4);

        return mat;
    }

    const float& mat4ValueAt(const glm::mat4& m, int l, int c) {
        return glm::value_ptr(m)[4 * c + l];
    }

    float& mat4ValueAt(glm::mat4& m, int l, int c) {
        return glm::value_ptr(m)[4 * c + l];
    }

    const float& mat3ValueAt(const glm::mat3& m, int l, int c) {
        return glm::value_ptr(m)[3 * c + l];
    }

    float& mat3ValueAt(glm::mat3& m, int l, int c) {
        return glm::value_ptr(m)[3 * c + l];
    }

    GLObjectCreationException::GLObjectCreationException(GLObjectType t) :
    type(t) {}

    const char *GLObjectCreationException::what() const throw() {
        switch (type) {
            case GLOBJECT_BUFFER:           return "Can't create additional buffer";
            case GLOBJECT_VERTEXARRAY:      return "Can't create additional vertex array";
            case GLOBJECT_FRAMEBUFFER:      return "Can't create additional framebuffer";
            case GLOBJECT_RENDERBUFFER:     return "Can't create additional renderbuffer";
            case GLOBJECT_SHADER:           return "Can't create additional shader";
            case GLOBJECT_SHADERPROGRAM:    return "Can't create additional shader program";
            case GLOBJECT_TEXTURE:          return "Can't create additional texture";
            case GLOBJECT_CLOBJECT:         return "OpenCL failed to create object";
            default:                        return "Can't create object";
        }
    }

    ShaderBuildingException::ShaderBuildingException(ShaderBuildingErrorType t, std::string il) :
        type(t),
        infoLog(il) {}
    
    const char *ShaderBuildingException::what() const throw() {
        switch (type) {
            case SHADERBUILDING_COMPILATION:    return "failed to compile shader";
            case SHADERBUILDING_LINK:           return "failed to link shaders";
            default:                            return "Error during shader building";
        }
    }

    FileLoadingException::FileLoadingException(std::string message) {
        _customMessage = "File loading error: " + message;
    }
    
    const char *FileLoadingException::what() const throw() {
        return _customMessage.c_str();
    }

    InvalidDataException::InvalidDataException(std::string message) {
        _customMessage = "Data error: " + message;
    }

    const char *InvalidDataException::what() const throw() {
        return _customMessage.c_str();
    }
}
