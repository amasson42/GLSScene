//
//  GLSFramebuffer.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSFramebuffer.hpp"

namespace GLS {

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

}
