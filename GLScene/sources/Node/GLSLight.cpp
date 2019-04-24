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
    _type(light_unused), _position(0), _color(1), _specular(1), _intensity(1),
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


    LightType Light::type() const {
        return _type;
    }

    void Light::setType(LightType type) {
        _type = type;
    }

    glm::vec3 Light::position() const {
        return _position;
    }

    void Light::setPosition(glm::vec3 position) {
        _position = position;
    }

    glm::vec3 Light::color() const {
        return _color;
    }

    void Light::setColor(glm::vec3 color) {
        _color = color;
    }

    glm::vec3 Light::specular() const {
        return _specular;
    }

    void Light::setSpecular(glm::vec3 specular) {
        _specular = specular;
    }

    GLfloat Light::intensity() const {
        return _intensity;
    }

    void Light::setIntensity(GLfloat intensity) {
        _intensity = intensity;
    }

    glm::vec3 Light::attenuation() const {
        return _attenuation;
    }

    void Light::setAttenuation(glm::vec3 attenuation) {
        _attenuation = attenuation;
    }

    glm::vec3 Light::direction() const {
        return _direction;
    }

    void Light::setDirection(glm::vec3 direction) {
        _direction = direction;
    }

    GLfloat Light::angle() const {
        return _angle;
    }

    void Light::setAngle(GLfloat angle) {
        _angle = angle;
    }

    GLfloat Light::coneAttenuation() const {
        return _cone_attenuation;
    }

    void Light::setConeAttenuation(GLfloat coneAttenuation) {
        _cone_attenuation = coneAttenuation;
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

    Light Light::transformedBy(glm::mat4 transform) const {
        Light l(*this);
        l._position = transform * glm::vec4(_position, 1.0);
        l._direction = transform * glm::vec4(_direction, 0.0);
        return l;
    }

}
