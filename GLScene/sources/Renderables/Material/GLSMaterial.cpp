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
    specular(0),
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

}
