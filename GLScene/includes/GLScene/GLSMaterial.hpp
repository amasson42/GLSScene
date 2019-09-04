//
//  GLSMaterial.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSMaterial_h
#define GLSMaterial_h

#include "GLSStructs.hpp"
#include "GLSTexture.hpp"
#include "GLSShader.hpp"

namespace GLS {

    class Material {

        public:

        glm::vec3 diffuse;
        glm::vec3 specular;
        GLfloat roughness;
        GLfloat metalness;
        glm::vec3 occlusion;
        GLfloat shininess;

        std::shared_ptr<Texture> texture_diffuse;
        std::shared_ptr<Texture> texture_specular;
        std::shared_ptr<Texture> texture_roughness;
        std::shared_ptr<Texture> texture_metalness;
        std::shared_ptr<Texture> texture_occlusion;
        std::shared_ptr<Texture> texture_shininess;
        std::shared_ptr<Texture> texture_normal;
        std::shared_ptr<Texture> texture_mask;

        Transform2D diffuse_transform;
        Transform2D specular_transform;
        Transform2D roughness_transform;
        Transform2D metalness_transform;
        Transform2D occlusion_transform;
        Transform2D shininess_transform;
        Transform2D normal_transform;
        Transform2D mask_transform;

        Material();
        Material(const Material& copy);
        virtual ~Material();

        Material& operator=(const Material& copy);


        // Utilities

        void sendUniformsToShaderProgram(std::shared_ptr<ShaderProgram> program) const;

        static std::shared_ptr<Material> loadFromAiMaterial(aiMaterial *material, const std::string& directory);

    };

}

#endif /* GLSMaterial_h */
