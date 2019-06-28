//
//  GLSSkybox.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "stb_image.h"
#include "GLSSkybox.hpp"

namespace GLS {

   GLuint Skybox::_cubebuffer = 0;
   static GLuint _cubeVbo = 0;

    void Skybox::_createCubeBuffer() {
        float skyboxVertices[] = {
            -1.0f,  1.0f, -1.0f,    -1.0f, -1.0f, -1.0f,    1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,     1.0f,  1.0f, -1.0f,     -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,    -1.0f, -1.0f, -1.0f,    -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,    -1.0f,  1.0f,  1.0f,    -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,     1.0f, -1.0f,  1.0f,     1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,     1.0f,  1.0f, -1.0f,     1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,    -1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,     1.0f, -1.0f,  1.0f,     -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,    1.0f,  1.0f, -1.0f,     1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,     -1.0f,  1.0f,  1.0f,    -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,    -1.0f, -1.0f,  1.0f,    1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,     -1.0f, -1.0f,  1.0f,    1.0f, -1.0f,  1.0f
        };

        glGenVertexArrays(1, &_cubebuffer);
        if (_cubebuffer == 0)
            return;
        glGenBuffers(1, &_cubeVbo);
        if (_cubeVbo == 0) {
            glDeleteVertexArrays(1, &_cubebuffer);
            return;
        }
        glBindVertexArray(_cubebuffer);

        glBindBuffer(GL_ARRAY_BUFFER, _cubeVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Skybox::_destroyCubeBuffer() {
        glDeleteBuffers(1, &_cubeVbo);
        glDeleteVertexArrays(1, &_cubebuffer);
    }

    Skybox::Skybox(std::vector<std::string> faces) {
        if (faces.size() != 6)
            throw InvalidDataException("skybox needs exactly 6 textures");
        glGenTextures(1, &_textureid);
        if (_textureid == 0)
            throw GLObjectCreationException(GLOBJECT_TEXTURE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _textureid);

        int width, height, bpp;
        for (size_t i = 0; i < faces.size(); i++) {
            uint8_t *data = stbi_load(faces[i].c_str(), &width, &height, &bpp, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGB, width, height,
                             0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            } else {
                glDeleteTextures(1, &_textureid);
                throw FileLoadingException("can't load texture from file " + faces[i]);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    Skybox::~Skybox() {
        glDeleteTextures(1, &_textureid);
    }

    std::pair<glm::vec3, glm::vec3> Skybox::getBounds(glm::mat4 transform) const {
        (void)transform;
        return std::pair<glm::vec3, glm::vec3>(glm::vec3(-1), glm::vec3(1));
    }

}