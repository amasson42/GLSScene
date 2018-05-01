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
    
    Texture::Texture(std::string filename, GLenum format) {
        _data = stbi_load(filename.c_str(), &_width, &_height, &_bpp, 0);
        if (_data == NULL) {
            // TODO: throw can't create texture
        }
        glGenTextures(1, &_buffer);
        glBindTexture(GL_TEXTURE_2D, _buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, _data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    
    Texture::~Texture() {
        clearData();
        glDeleteTextures(1, &_buffer);
    }
    
    void Texture::clearData() {
        if (_data)
            free(_data);
        _data = NULL;
    }
    
    GLuint Texture::buffer() const {
        return _buffer;
    }

    void Texture::setParameter(GLenum pname, GLint param) {
        glBindTexture(GL_TEXTURE_2D, _buffer);
        glTexParameteri(GL_TEXTURE_2D, pname, param);
    }
    
}
