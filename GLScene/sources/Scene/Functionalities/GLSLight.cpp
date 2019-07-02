//
//  GLSLight.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSLight.hpp"

#ifndef M_PI
# define M_PI 3.14159265359
#endif

namespace GLS {
    
    Light::Light() :
    type(light_unused), color(1), specular(1), intensity(1),
    attenuation(1, 0, 0),
    angle(M_PI / 4), cone_attenuation(M_PI / 6),
    width(10.0), height(10.0),
    cast_shadow(false),
    cast_shadow_clip_near(0.1), cast_shadow_clip_far(100.0),
    cast_shadow_map_size_width(1024), cast_shadow_map_size_height(1024),
    _projection(1), _view(1),
    _caster_index(-1)
    {

    }


    // Utilities

    void Light::sendUniformsToShaderProgram(std::shared_ptr<ShaderProgram> program, int light_index) const {
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
        if (l.cast_shadow) {
            if (l.type == light_spot) {
                l._projection = Camera(l.angle * 2, 1.0, l.cast_shadow_clip_near, l.cast_shadow_clip_far).projectionMatrix();
            } else if (l.type == light_directional) {
                l._projection = glm::ortho(-l.width / 2, l.height / 2,
                                            -l.height / 2, l.height / 2,
                                            l.cast_shadow_clip_near, l.cast_shadow_clip_far);
            }
        }

        return l;
    }

    LightCaster::LightCaster(Light l) {
        light = l;
        depth_map = std::make_shared<Framebuffer>(
            l.cast_shadow_map_size_width, l.cast_shadow_map_size_height,
            false,
            GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
        depth_map->bind();
        depth_map->texture()->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        depth_map->texture()->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        depth_map->unbind();
    }


    // Utilities

    void LightCaster::sendUniformsToShaderProgram(std::shared_ptr<ShaderProgram> program, int index) const {
        glm::mat4 vp = light._projection * glm::inverse(light._view);
        program->use();
        #ifdef SCHOOL_DUMPS
        glUniformMatrix4fv(program->getLocation("light_casters_vp[" + std::to_string(index) + "]"), 1, GL_FALSE, glm::value_ptr(vp));
        depth_map->texture()->sendUniformsToShaderProgram(program, "light_casters_depth_map[" + std::to_string(index) + "]", 12 + index);
        #else
        glUniformMatrix4fv(program->getLocation("light_casters[" + std::to_string(index) + "].vp"), 1, GL_FALSE, glm::value_ptr(vp));
        depth_map->texture()->sendUniformsToShaderProgram(program, "light_casters[" + std::to_string(index) + "].depth_map", 12 + index);
        #endif
    }

}
