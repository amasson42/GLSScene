//
//  GLSLight.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSLight_h
#define GLSLight_h

#include "GLScene.hpp"

namespace GLS {

    enum LightType {
        light_unused = 0,
        light_directional = 1,
        light_spot = 2,
        light_point = 3,
        light_ambiant = 4
    };

    class Light {

        LightType _type;
        glm::vec3 _position;
        glm::vec3 _color;
        glm::vec3 _specular;
        GLfloat _intensity;
        glm::vec3 _attenuation;
        glm::vec3 _direction;
        GLfloat _angle;
        GLfloat _cone_attenuation;

    public:
        Light();
        Light(const Light& copy);
        virtual ~Light();

        Light& operator=(const Light& copy);

        LightType type() const;
        void setType(LightType type);

        glm::vec3 position() const;
        void setPosition(glm::vec3 position);

        glm::vec3 color() const;
        void setColor(glm::vec3 color);

        glm::vec3 specular() const;
        void setSpecular(glm::vec3 specular);

        GLfloat intensity() const;
        void setIntensity(GLfloat intensity);

        glm::vec3 attenuation() const;
        void setAttenuation(glm::vec3 attenuation);

        glm::vec3 direction() const;
        void setDirection(glm::vec3 direction);

        GLfloat angle() const;
        void setAngle(GLfloat angle);

        GLfloat coneAttenuation() const;
        void setConeAttenuation(GLfloat cone_attenuation);


        // Utilities

        void sendUniformToShaderProgram(std::shared_ptr<ShaderProgram> program, int light_index) const;
        Light transformedBy(glm::mat4 transform) const;

    };

}

#endif /* GLSLight_h */
