//
//  GLSStructs.hpp
//  GLScene
//
//  Created by Arthur Masson on 16/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSStructs_h
#define GLSStructs_h

// TODO: why do we have to do this shit
// #define SCHOOL_DUMPS

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <algorithm>
#include <future>
#include <sys/stat.h>
#include <iomanip>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "GLSTransform.hpp"

#include "CLDevice.hpp"

std::ostream& operator<<(std::ostream& stream, const glm::mat4& m);
std::ostream& operator<<(std::ostream& stream, const glm::mat3& m);
std::ostream& operator<<(std::ostream& stream, const glm::vec3& v);
std::ostream& operator<<(std::ostream& stream, const glm::vec2& v);
std::ostream& operator<<(std::ostream& stream, const glm::ivec3& v);
std::ostream& operator<<(std::ostream& stream, const glm::ivec2& v);
std::ostream& operator<<(std::ostream& stream, const glm::quat& q);

namespace GLS {

    std::shared_ptr<CLD::GPUDevice> getSharedDevice();
    void glsInit(void* addr);
    void glsDeinit();

    class ShaderProgram;

    struct RenderUniforms {
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
        glm::vec3 camera_position;
        float fog_near;
        float fog_far;
        glm::vec3 fog_color;

        RenderUniforms();

        RenderUniforms(const glm::mat4& p,
                       const glm::mat4& v,
                       const glm::mat4& m,
                       const glm::vec3& c,
                       float fn,
                       float ff,
                       const glm::vec3& fc);
        
        void sendUniformsToShaderProgram(std::shared_ptr<ShaderProgram> program) const;
    };

    enum GLObjectType {
        GLOBJECT_UNKNOWN,
        GLOBJECT_BUFFER,
        GLOBJECT_VERTEXARRAY,
        GLOBJECT_FRAMEBUFFER,
        GLOBJECT_RENDERBUFFER,
        GLOBJECT_SHADER,
        GLOBJECT_SHADERPROGRAM,
        GLOBJECT_TEXTURE,
        GLOBJECT_CLOBJECT
    };

    class GLObjectCreationException : public std::exception {
        public:
        const GLObjectType type;
        GLObjectCreationException(GLObjectType type);
        const char *what() const throw();
    };

    enum ShaderBuildingErrorType {
        SHADERBUILDING_COMPILATION,
        SHADERBUILDING_LINK
    };

    class ShaderBuildingException : public std::exception {
        public:
        const ShaderBuildingErrorType type;
        const std::string infoLog;
        ShaderBuildingException(ShaderBuildingErrorType type, std::string infoLog);
        const char *what() const throw();
    };

    class FileLoadingException : public std::exception {
        std::string _customMessage;
        public:
        FileLoadingException(std::string message);
        const char *what() const throw();
    };

    class InvalidDataException : public std::exception {
        std::string _customMessage;
        public:
        InvalidDataException(std::string message);
        const char *what() const throw();
    };

}

#endif /* GLSStructs_h */
