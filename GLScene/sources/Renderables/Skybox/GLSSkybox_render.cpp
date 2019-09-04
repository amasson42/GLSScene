//
//  GLSSkybox_render.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSSkybox.hpp"
#include "GLSScene.hpp"

namespace GLS {

    void Skybox::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
        (void)scene;
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramSkybox();
        program->use();

        RenderUniforms centeredUniforms(uniforms);
        centeredUniforms.view = glm::mat4(glm::mat3(uniforms.view));
        centeredUniforms.sendUniformsToShaderProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _textureid);
        glUniform1i(program->getLocation("skybox"), 0);

        glBindVertexArray(Skybox::_cubebuffer);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
    }

}
