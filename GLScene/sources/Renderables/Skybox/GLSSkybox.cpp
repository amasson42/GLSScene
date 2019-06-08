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

    const char* Skybox::LoadingException::what() const throw() {
        return "can't load the texture";
    }

    const char* Skybox::CreationException::what() const throw() {
        return "can't create the skybox";
    }

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
            throw CreationException();
        glGenTextures(1, &_textureid);
        if (_textureid == 0)
            throw CreationException();
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
                throw LoadingException();
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

    void Skybox::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
        return;
        (void)scene;
        glDepthFunc(GL_LEQUAL);
        std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramSkybox();
        program->use();
        glm::mat4 view = glm::mat4(glm::mat3(uniforms.view));
        glUniformMatrix4fv(program->getLocation("u_mat_projection"), 1, GL_FALSE, glm::value_ptr(uniforms.projection));
        glUniformMatrix4fv(program->getLocation("u_mat_view"), 1, GL_FALSE, glm::value_ptr(view));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _textureid);
        glUniform1i(program->getLocation("skybox"), 0);

        glBindVertexArray(Skybox::_cubebuffer);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
    }

        // if (texture_diffuse) {
        //     glActiveTexture(GL_TEXTURE0);
        //     glBindTexture(GL_TEXTURE_2D, texture_diffuse->buffer());
        //     glUniform1i(program->getLocation("texture_diffuse"), 0);
        //     texturebitmask |= (1 << 0);
        //     glm::mat3 mat = diffuse_transform.matrix();
        //     glUniformMatrix3fv(program->getLocation("diffuse_transform"), 1, GL_FALSE, glm::value_ptr(mat));
        // }
    
    std::pair<glm::vec3, glm::vec3> Skybox::getBounds(glm::mat4 transform) const {
        (void)transform;
        return std::pair<glm::vec3, glm::vec3>(glm::vec3(-1), glm::vec3(1));
    }

    std::string Skybox::shaderUniformsVertex() {
        return
        "layout (location = 0) in vec3 position;\n"
        "\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "\n"
        "out VS_OUT {\n"
        "    vec3 uv;\n"
        "} vs_out;\n"
        "\n";
    }

    std::string Skybox::shaderUniformsGeometry() {
        return
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "\n"
        "in VS_OUT {\n"
        "    vec3 uv;\n"
        "} gs_in;\n"
        "\n"
        "out VS_OUT {\n"
        "    vec3 uv;\n"
        "} gs_out;\n"
        "\n";
    }

    std::string Skybox::shaderUniformsFragment() {
        return
        "in VS_OUT {\n"
        "    vec3 uv;\n"
        "} fs_in;\n"
        "\n"
        "uniform samplerCube u_skybox;\n"
        "\n"
        "out vec4 FragColor;\n"
        "\n";
    }

}