//
//  GLSSkybox_render.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSSkybox.hpp"

namespace GLS {

    void Skybox::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
        (void)scene;
        glDepthMask(GL_FALSE);
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
        glDepthMask(GL_TRUE);
    }

}
