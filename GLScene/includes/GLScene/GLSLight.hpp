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

    struct Light {

        LightType type;

        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 specular;
        GLfloat intensity;
        glm::vec3 attenuation;

        glm::vec3 direction; // for directional and spot
        GLfloat angle; // for spot
        GLfloat cone_attenuation; // for spot

        Light();

        // Utilities

        void sendUniformToShaderProgram(std::shared_ptr<ShaderProgram> program, int light_index) const;
        Light transformedBy(glm::mat4 transform) const;

    };

}

#endif /* GLSLight_h */
