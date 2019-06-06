//
//  GLSFramebuffer.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSFramebuffer.hpp"

namespace GLS {
    
    const char *Framebuffer::CreationException::what() const throw() {
        return "can't create framebuffer";
    }

    GLuint Framebuffer::_rectbuffer = 0;
    static GLuint _rectVbo = 0;

    void Framebuffer::_createRectBuffer() {
        float rectVertices[] = {
            -1.0f,  1.0f,
            -1.0f, -1.0f,
            1.0f, -1.0f,
            -1.0f,  1.0f,
            1.0f, -1.0f,
            1.0f,  1.0f
        };
        glGenBuffers(1, &_rectVbo);
        if (_rectVbo == 0)
            return;
        glGenVertexArrays(1, &_rectbuffer);
        if (_rectbuffer == 0) {
            glDeleteBuffers(1, &_rectVbo);
            return;
        }
        glBindVertexArray(_rectbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _rectVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), &rectVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
    }

    void Framebuffer::_destroyRectBuffer() {
        glDeleteVertexArrays(1, &_rectbuffer);
        glDeleteBuffers(1, &_rectVbo);
    }

    Framebuffer::Framebuffer(GLsizei width, GLsizei height, bool createRenderbuffer, GLint format, GLenum type, GLenum attachment) throw(CreationException) :
        _framebuffer(0), _renderbuffer(0),
        _colorTexture(nullptr),
        _program(nullptr)
    {
        glGenFramebuffers(1, &_framebuffer);
        if (_framebuffer == 0) {
            throw CreationException();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
        
        try {
            _colorTexture = std::make_shared<Texture>(width, height, format, type);
        } catch (std::exception& e) {
            glDeleteFramebuffers(1, &_framebuffer);
            throw CreationException();
        }
        _colorTexture->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        _colorTexture->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
            GL_TEXTURE_2D, _colorTexture->buffer(), 0);

        if (createRenderbuffer) {
            glGenRenderbuffers(1, &_renderbuffer);
            if (_renderbuffer == 0) {
                glDeleteFramebuffers(1, &_framebuffer);
                throw CreationException();
            }
            glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _renderbuffer);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                glDeleteFramebuffers(1, &_framebuffer);
                glDeleteRenderbuffers(1, &_renderbuffer);
                throw CreationException();
            }
        }

        unbind();
    }

    Framebuffer::~Framebuffer() {
        glDeleteFramebuffers(1, &_framebuffer);
        if (_renderbuffer != 0)
            glDeleteRenderbuffers(1, &_renderbuffer);
    }

    GLsizei Framebuffer::width() const {
        return _colorTexture->width();
    }

    GLsizei Framebuffer::height() const {
        return _colorTexture->height();
    }

    std::shared_ptr<Texture> Framebuffer::texture() {
        return _colorTexture;
    }

    std::pair<glm::vec3, glm::vec3> Framebuffer::getBounds(glm::mat4 transform) const {
        (void)transform;
        return std::make_pair<glm::vec3, glm::vec3>(glm::vec3(0), glm::vec3(0));
    }

    void Framebuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    }

    void Framebuffer::unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::setProgram(std::shared_ptr<ShaderProgram> program) {
        _program = program;
    }

    void Framebuffer::renderInContext(Scene& scene, const RenderUniforms& uniforms) {

        (void)scene;

        std::shared_ptr<ShaderProgram> program;
        if (_program == nullptr)
            program = ShaderProgram::standardShaderProgramScreenTexture();
        else
            program = _program;
        program->use();

        glEnable(GL_DEPTH_TEST | GL_STENCIL_TEST);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _colorTexture->buffer());
        glUniform1i(program->getLocation("screen_texture"), 0);

        uniforms.sendUniformsToShaderProgram(program);

        glBindVertexArray(Framebuffer::_rectbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


    // Shader Uniforms

    std::string Framebuffer::shaderUniformsVertex() {
        return
        "layout (location = 0) in vec2 position;\n"
        "\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "uniform vec3 u_camera_position;\n"
        "\n"
        "out VS_OUT {\n"
        "    vec2 uv;\n"
        "} vs_out;\n"
        "\n";
    }

    std::string Framebuffer::shaderUniformsGeometry() {
        return
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "uniform vec3 u_camera_position;\n"
        "\n"
        "in VS_OUT {\n"
        "    vec2 uv;\n"
        "} gs_in;\n"
        "\n"
        "out VS_OUT {\n"
        "    vec2 uv;\n"
        "} gs_out;\n"
        "\n";
    }

    std::string Framebuffer::shaderUniformsFragment() {
        return
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "uniform vec3 u_camera_position;\n"
        "\n"
        "in VS_OUT {\n"
        "    vec2 uv;\n"
        "} fs_in;\n"
        "\n"
        "uniform sampler2D screen_texture;\n"
        "\n"
        "out vec4 FragColor;\n"
        "\n";
    }

}
