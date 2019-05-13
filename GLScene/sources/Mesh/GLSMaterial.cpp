//
//  GLSMaterial.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {

    Material::Material() :
    diffuse(1),
    specular(1),
    roughness(1),
    metalness(1),
    occlusion(1),
    shininess(1)
    {
        
    }

    Material::Material(const Material& copy) :
    diffuse(copy.diffuse),
    specular(copy.specular),
    roughness(copy.roughness),
    metalness(copy.metalness),
    occlusion(copy.occlusion),
    shininess(copy.shininess),
    texture_diffuse(copy.texture_diffuse),
    texture_specular(copy.texture_specular),
    texture_roughness(copy.texture_roughness),
    texture_metalness(copy.texture_metalness),
    texture_occlusion(copy.texture_occlusion),
    texture_shininess(copy.texture_shininess),
    texture_normal(copy.texture_normal),
    texture_mask(copy.texture_mask),
    diffuse_transform(copy.diffuse_transform),
    specular_transform(copy.specular_transform),
    roughness_transform(copy.roughness_transform),
    metalness_transform(copy.metalness_transform),
    occlusion_transform(copy.occlusion_transform),
    shininess_transform(copy.shininess_transform),
    normal_transform(copy.normal_transform),
    mask_transform(copy.mask_transform)
    {

    }

    Material::~Material() {

    }

    Material& Material::operator=(const Material& copy) {

        diffuse = copy.diffuse;
        specular = copy.specular;
        roughness = copy.roughness;
        metalness = copy.metalness;
        occlusion = copy.occlusion;
        shininess = copy.shininess;

        texture_diffuse = copy.texture_diffuse;
        texture_specular = copy.texture_specular;
        texture_roughness = copy.texture_roughness;
        texture_metalness = copy.texture_metalness;
        texture_occlusion = copy.texture_occlusion;
        texture_shininess = copy.texture_shininess;
        texture_normal = copy.texture_normal;
        texture_mask = copy.texture_mask;

        diffuse_transform = copy.diffuse_transform;
        specular_transform = copy.specular_transform;
        roughness_transform = copy.roughness_transform;
        metalness_transform = copy.metalness_transform;
        occlusion_transform = copy.occlusion_transform;
        shininess_transform = copy.shininess_transform;
        normal_transform = copy.normal_transform;
        mask_transform = copy.mask_transform;

        return *this;
    }

    void Material::sendUniformToShaderProgram(std::shared_ptr<ShaderProgram> program) const {

        program->use();

        glUniform3f(program->getLocation("material.diffuse"), diffuse.x, diffuse.y, diffuse.z);
        glUniform3f(program->getLocation("material.specular"), specular.x, specular.y, specular.z);
        glUniform1f(program->getLocation("material.roughness"), roughness);
        glUniform1f(program->getLocation("material.metalness"), metalness);
        glUniform3f(program->getLocation("material.occlusion"), occlusion.x, occlusion.y, occlusion.z);
        glUniform1f(program->getLocation("material.shininess"), shininess);

        int texturebitmask = 0;
        if (texture_diffuse) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_diffuse->buffer());
            glUniform1i(program->getLocation("texture_diffuse"), 0);
            texturebitmask |= (1 << 0);
            glm::mat3 mat = diffuse_transform.matrix();
            glUniformMatrix3fv(program->getLocation("diffuse_transform"), 1, GL_FALSE, glm::value_ptr(mat));
        }
        if (texture_specular) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_specular->buffer());
            glUniform1i(program->getLocation("texture_specular"), 1);
            texturebitmask |= (1 << 1);
            glm::mat3 mat = specular_transform.matrix();
            glUniformMatrix3fv(program->getLocation("specular_transform"), 1, GL_FALSE, glm::value_ptr(mat));
        }
        if (texture_roughness) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, texture_roughness->buffer());
            glUniform1i(program->getLocation("texture_roughness"), 2);
            texturebitmask |= (1 << 2);
            glm::mat3 mat = roughness_transform.matrix();
            glUniformMatrix3fv(program->getLocation("roughness_transform"), 1, GL_FALSE, glm::value_ptr(mat));
        }
        if (texture_metalness) {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, texture_metalness->buffer());
            glUniform1i(program->getLocation("texture_metalness"), 3);
            texturebitmask |= (1 << 3);
            glm::mat3 mat = metalness_transform.matrix();
            glUniformMatrix3fv(program->getLocation("metalness_transform"), 1, GL_FALSE, glm::value_ptr(mat));
        }
        if (texture_occlusion) {
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, texture_occlusion->buffer());
            glUniform1i(program->getLocation("texture_occlusion"), 4);
            texturebitmask |= (1 << 4);
            glm::mat3 mat = occlusion_transform.matrix();
            glUniformMatrix3fv(program->getLocation("occlusion_transform"), 1, GL_FALSE, glm::value_ptr(mat));
        }
        if (texture_shininess) {
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, texture_shininess->buffer());
            glUniform1i(program->getLocation("texture_shininess"), 5);
            texturebitmask |= (1 << 5);
            glm::mat3 mat = shininess_transform.matrix();
            glUniformMatrix3fv(program->getLocation("shininess_transform"), 1, GL_FALSE, glm::value_ptr(mat));
        }
        if (texture_normal) {
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, texture_normal->buffer());
            glUniform1i(program->getLocation("texture_normal"), 6);
            texturebitmask |= (1 << 6);
            glm::mat3 mat = normal_transform.matrix();
            glUniformMatrix3fv(program->getLocation("normal_transform"), 1, GL_FALSE, glm::value_ptr(mat));
        }
        if (texture_mask) {
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, texture_mask->buffer());
            glUniform1i(program->getLocation("texture_mask"), 7);
            texturebitmask |= (1 << 7);
            glm::mat3 mat = mask_transform.matrix();
            glUniformMatrix3fv(program->getLocation("mask_transform"), 1, GL_FALSE, glm::value_ptr(mat));
        }
        glUniform1i(program->getLocation("texturebitmask"), texturebitmask);
    }

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
