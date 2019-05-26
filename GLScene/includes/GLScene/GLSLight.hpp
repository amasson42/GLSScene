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

        static bool lightTypeCanCastShadow(LightType type) {
            return type == light_spot || type == light_directional;
        }

        LightType type;

        glm::vec3 color;
        glm::vec3 specular;
        GLfloat intensity;
        glm::vec3 attenuation;

        GLfloat angle; // for spot
        GLfloat cone_attenuation; // for spot
        GLfloat width; // for directional
        GLfloat height; // for directional

        bool cast_shadow; // for spot and directional
        // TODO: cast shadow for point nodes with cubemap

        GLfloat cast_shadow_clip_near;
        GLfloat cast_shadow_clip_far;
        size_t cast_shadow_map_size_width;
        size_t cast_shadow_map_size_height;

        private:
        glm::mat4 _projection;
        glm::mat4 _view;
        int _caster_index;
        friend class Scene;
        friend struct LightCaster;
        public:

        Light();

        // Utilities

        void sendUniformToShaderProgram(std::shared_ptr<ShaderProgram> program, int light_index) const;
        Light transformedBy(glm::mat4 transform) const;

    };

    struct LightCaster {

        std::shared_ptr<Framebuffer> depth_map;
        Light light;

        LightCaster(Light l);

        // Utilities

        void sendUniformToShaderProgram(std::shared_ptr<ShaderProgram> program, int caster_index) const;

    };

}

#endif /* GLSLight_h */
