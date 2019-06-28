//
//  GLSTexture.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSTexture.hpp"

namespace GLS {

    Texture::Texture(GLsizei width, GLsizei height, GLint format, GLenum type) {
        _width = width;
        _height = height;
        glGenTextures(1, &_buffer);
        if (_buffer == 0) {
            throw GLObjectCreationException(GLOBJECT_TEXTURE);
        }
        glBindTexture(GL_TEXTURE_2D, _buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture::Texture(std::string path) {
        int bpp;
        int width, height;
        uint8_t *data = stbi_load(path.c_str(), &width, &height, &bpp, 0);
        _width = width;
        _height = height;
        if (data == NULL) {
            throw FileLoadingException("can't load texture from file " + path);
        }
        GLuint format;
        switch (bpp) {
            case 1: format = GL_RED;  break;
            case 3: format = GL_RGB;  break;
            case 4: format = GL_RGBA; break;
            default:
                stbi_image_free(data);
                throw InvalidDataException("unknown format of texture in file " + path);
        }
        glGenTextures(1, &_buffer);
        if (_buffer == 0) {
            stbi_image_free(data);
            throw GLObjectCreationException(GLOBJECT_TEXTURE);
        }
        glBindTexture(GL_TEXTURE_2D, _buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    
    Texture::~Texture() {
        glDeleteTextures(1, &_buffer);
    }
    
    GLuint Texture::buffer() const {
        return _buffer;
    }

    void Texture::sendUniformsToShaderProgram(std::shared_ptr<ShaderProgram> program, std::string uniformName, int textureIndex) const {
        program->use();
        glActiveTexture(GL_TEXTURE0 + textureIndex);
        glBindTexture(GL_TEXTURE_2D, _buffer);
        glUniform1i(program->getLocation(uniformName), textureIndex);
    }

    void Texture::setParameter(GLenum pname, GLint param) {
        glBindTexture(GL_TEXTURE_2D, _buffer);
        glTexParameteri(GL_TEXTURE_2D, pname, param);
    }
    
    void Texture::setParameters(std::vector<GLenum> pnames, std::vector<GLint> params) {
        glBindTexture(GL_TEXTURE_2D, _buffer);
        if (pnames.size() == params.size()) {
            for (size_t i = 0; i < pnames.size(); i++) {
                glTexParameteri(GL_TEXTURE_2D, pnames[i], params[i]);
            }
        }
    }

    GLsizei Texture::width() const {
        return _width;
    }

    GLsizei Texture::height() const {
        return _height;
    }

}
