//
//  GLSLight.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSLight.hpp"

namespace GLS {
    
    // enum LightType : int {
    //     unused = 0,
    //     directional = 1,
    //     spot = 2,
    //     pointlight = 3,
    //     ambiantlight = 4
    // };

    // class Light {

    //     LightType _type;
    //     glm::vec3 _position;
    //     glm::vec3 _color;
    //     glm::vec3 _specular;
    //     GLfloat _intensity;
    //     glm::vec3 _attenuation;
    //     glm::vec3 _direction;
    //     GLfloat _angle;
    //     GLfloat _cone_attenuation;

    // public:
    //     Light();
    //     Light(const Light& copy);
    //     virtual ~Light();

    //     Light& operator=(const Light& copy);


    //     // Utilities

    //     void sendUniformToShaderProgram(std::shared_ptr<ShaderProgram> program) const;

    // };

    Light::Light() :
    _type(unused), _position(0), _color(1), _specular(1), _intensity(1),
    _attenuation(1, 0, 0), _direction(0, 0, -1),
    _angle(1.5), _cone_attenuation(1)
    {

    }

    Light::Light(const Light& copy) :
    _type(copy._type),
    _position(copy._position),
    _color(copy._color),
    _specular(copy._specular),
    _intensity(copy._intensity),
    _attenuation(copy._attenuation),
    _direction(copy._direction),
    _angle(copy._angle),
    _cone_attenuation(copy._cone_attenuation)
    {

    }
    
    Light::~Light() {

    }

    Light& Light::operator=(const Light& copy) {
        _type = copy._type;
        _position = copy._position;
        _color = copy._color;
        _specular = copy._specular;
        _intensity = copy._intensity;
        _attenuation = copy._attenuation;
        _direction = copy._direction;
        _angle = copy._angle;
        _cone_attenuation = copy._cone_attenuation;
        return *this;
    }


    // Utilities

    void Light::sendUniformToShaderProgram(std::shared_ptr<ShaderProgram> program, int light_index) const {
        program->use();

        std::string slight = "lights[" + std::to_string(light_index) + "]";
        glUniform1i(program->getLocation(slight + ".type"), static_cast<int>(_type));
        glUniform3f(program->getLocation(slight + ".position"), _position.x, _position.y, _position.z);
        glUniform3f(program->getLocation(slight + ".color"), _color.x, _color.y, _color.z);
        glUniform3f(program->getLocation(slight + ".specular"), _specular.x, _specular.y, _specular.z);
        glUniform1f(program->getLocation(slight + ".intensity"), _intensity);
        glUniform3f(program->getLocation(slight + ".attenuation"), _attenuation.x, _attenuation.y, _attenuation.z);
        glUniform3f(program->getLocation(slight + ".direction"), _direction.x, _direction.y, _direction.z);
        glUniform1f(program->getLocation(slight + ".angle"), _angle);
        glUniform1f(program->getLocation(slight + ".cone_attenuation"), _cone_attenuation);
    }

}
