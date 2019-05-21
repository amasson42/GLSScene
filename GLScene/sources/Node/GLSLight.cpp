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
    type(light_unused), position(0), color(1), specular(1), intensity(1),
    attenuation(1, 0, 0), direction(0, 0, -1),
    angle(1.5), cone_attenuation(1)
    {

    }


    // Utilities

    void Light::sendUniformToShaderProgram(std::shared_ptr<ShaderProgram> program, int light_index) const {
        program->use();

        std::string slight = "lights[" + std::to_string(light_index) + "]";
        glUniform1i(program->getLocation(slight + ".type"), static_cast<int>(type));
        glUniform3f(program->getLocation(slight + ".position"), position.x, position.y, position.z);
        glUniform3f(program->getLocation(slight + ".color"), color.x, color.y, color.z);
        glUniform3f(program->getLocation(slight + ".specular"), specular.x, specular.y, specular.z);
        glUniform1f(program->getLocation(slight + ".intensity"), intensity);
        glUniform3f(program->getLocation(slight + ".attenuation"), attenuation.x, attenuation.y, attenuation.z);
        glUniform3f(program->getLocation(slight + ".direction"), direction.x, direction.y, direction.z);
        glUniform1f(program->getLocation(slight + ".angle"), angle);
        glUniform1f(program->getLocation(slight + ".cone_attenuation"), cone_attenuation);
    }

    Light Light::transformedBy(glm::mat4 transform) const {
        Light l(*this);
        l.position = transform * glm::vec4(position, 1.0);
        l.direction = transform * glm::vec4(direction, 0.0);
        return l;
    }

}
