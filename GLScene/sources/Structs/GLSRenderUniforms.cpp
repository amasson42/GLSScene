//
//  GLSRenderUniforms.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLScene.hpp"

namespace GLS {

    RenderUniforms::RenderUniforms() {
        projection = glm::mat4(1);
        view = glm::mat4(1);
        model = glm::mat4(1);
        camera_position = glm::vec3(0);
        fog_near = 80;
        fog_far = 100;
        fog_color = glm::vec3(0.1f);
    }

    RenderUniforms::RenderUniforms(const glm::mat4& p,
                                   const glm::mat4& v,
                                   const glm::mat4& m,
                                   const glm::vec3& c,
                                   float fn,
                                   float ff,
                                   const glm::vec3& fc) {
        projection = p;
        view = v;
        model = m;
        camera_position = c;
        fog_near = fn;
        fog_far = ff;
        fog_color = fc;
    }

    void RenderUniforms::sendUniformsToShaderProgram(std::shared_ptr<ShaderProgram> program) const {
        program->use();
        glUniformMatrix4fv(program->getLocation("u_mat_projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(program->getLocation("u_mat_view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(program->getLocation("u_mat_model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(program->getLocation("u_camera_position"), camera_position.x,
                                                               camera_position.y,
                                                               camera_position.z);
        glUniform1f(program->getLocation("u_fog_near"), fog_near);
        glUniform1f(program->getLocation("u_fog_far"), fog_far);
        glUniform3f(program->getLocation("u_fog_color"), fog_color.x,
                                                         fog_color.y,
                                                         fog_color.z);
    }

}
