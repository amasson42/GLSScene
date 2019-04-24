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
        unused = 0,
        directional = 1,
        spot = 2,
        pointlight = 3,
        ambiantlight = 4
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


        // Utilities

        void sendUniformToShaderProgram(std::shared_ptr<ShaderProgram> program, int light_index) const;

    };

}

#endif
