//
//  GLSLight.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSLight.hpp"

namespace GLS {
    
    Light::Light() :
    type(light_unused), color(1), specular(1), intensity(1),
    attenuation(1, 0, 0),
    angle(M_PI / 4), cone_attenuation(M_PI / 6),
    cast_shadow(false),
    _caster_index(-1)
    {

    }


    // Utilities

    void Light::sendUniformToShaderProgram(std::shared_ptr<ShaderProgram> program, int light_index) const {
        program->use();

        std::string slight = "lights[" + std::to_string(light_index) + "]";
        glUniform1i(program->getLocation(slight + ".type"), static_cast<int>(type));
        glUniform3f(program->getLocation(slight + ".color"), color.x, color.y, color.z);
        glUniform3f(program->getLocation(slight + ".specular"), specular.x, specular.y, specular.z);
        glUniform1f(program->getLocation(slight + ".intensity"), intensity);
        glUniform3f(program->getLocation(slight + ".attenuation"), attenuation.x, attenuation.y, attenuation.z);
        glUniform1f(program->getLocation(slight + ".angle"), angle);
        glUniform1f(program->getLocation(slight + ".cone_attenuation"), cone_attenuation);
        glUniform1i(program->getLocation(slight + ".caster_index"), _caster_index);
        glm::vec3 position = _view * glm::vec4(0, 0, 0, 1);
        glm::vec3 direction = _view * glm::vec4(0, 0, -1, 0);
        glUniform3f(program->getLocation(slight + ".position"), position.x, position.y, position.z);
        glUniform3f(program->getLocation(slight + ".direction"), direction.x, direction.y, direction.z);
    }

    Light Light::transformedBy(glm::mat4 transform) const {
        Light l(*this);
        l._view = transform;
        l._caster_index = -1;
        return l;
    }

    LightCaster::LightCaster(size_t width, size_t height) {
        depth_map = std::make_shared<Framebuffer>(width, height,
            GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
        depth_map->bind();
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        depth_map->unbind();
    }


    // Utilities

    void LightCaster::sendUniformToShaderProgram(std::shared_ptr<ShaderProgram> program, int index) const {
        program->use();
        std::string scaster = "light_casters[" + std::to_string(index) + "]";
        glUniformMatrix4fv(program->getLocation(scaster + ".vp"), 1, GL_FALSE, glm::value_ptr(light._view));
        glActiveTexture(GL_TEXTURE12 + index);
        glBindTexture(GL_TEXTURE_2D, depth_map->texture()->buffer());
        glUniform1i(program->getLocation(scaster + ".depth_map"), 12 + index);
    }

}
