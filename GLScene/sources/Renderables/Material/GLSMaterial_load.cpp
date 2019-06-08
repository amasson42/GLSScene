//
//  GLSMaterial_load.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {

    static std::shared_ptr<Texture> _loadTextureTypeFromMaterial(aiMaterial *material, aiTextureType type, const std::string& directory) {
        if (material->GetTextureCount(type) > 0) {
            aiString textureName;
            material->GetTexture(type, 0, &textureName);
            std::cout << "load texture with name: " << textureName.C_Str() << std::endl;
            std::string texturePath = directory + '/' + textureName.C_Str();
            try {
                return std::make_shared<Texture>(texturePath);
            } catch (std::exception& e) {
                std::cout << "Error during material loading with texture " << texturePath << " : " << e.what() << std::endl;
            }
        }
        return nullptr;
    }

    std::shared_ptr<Material> Material::loadFromAiMaterial(aiMaterial *material, const std::string& directory) {
        std::shared_ptr<Material> nMaterial = std::make_shared<Material>();
        
        aiColor3D diffuse;
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS) {
            nMaterial->diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
        }
        aiColor3D specular;
        if (material->Get(AI_MATKEY_COLOR_SPECULAR, specular) == AI_SUCCESS) {
            nMaterial->specular = glm::vec3(specular.r, specular.g, specular.b);
        }
        // GLfloat roughness;
        // GLfloat metalness;
        // glm::vec3 occlusion;
        float shininess;
        if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
            nMaterial->shininess = shininess;
        }

        nMaterial->texture_diffuse = _loadTextureTypeFromMaterial(material, aiTextureType_DIFFUSE, directory);
        nMaterial->texture_specular = _loadTextureTypeFromMaterial(material, aiTextureType_SPECULAR, directory);
        // texture_roughness
        // texture_metalness
        // texture_occlusion
        nMaterial->texture_shininess = _loadTextureTypeFromMaterial(material, aiTextureType_SHININESS, directory);
        nMaterial->texture_normal = _loadTextureTypeFromMaterial(material, aiTextureType_NORMALS, directory);
        nMaterial->texture_mask = _loadTextureTypeFromMaterial(material, aiTextureType_OPACITY, directory);
        return nMaterial;
    }

}
