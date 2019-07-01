//
//  GLSMesh_shader.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {

    std::string Mesh::shaderUniformsVertex() {
        return
        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec3 normal;\n"
        "layout (location = 2) in vec3 tangent;\n"
        "layout (location = 3) in vec3 bitangent;\n"
        "layout (location = 4) in vec2 uv;\n"
        "\n"
        "uniform mat3 u_mat_normal;\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "uniform vec3 u_camera_position;\n"
        "\n"
        "out VS_OUT {\n"
        "    vec4 position;\n"
        "    vec3 wposition;\n"
        "    vec2 uv;\n"
        "    vec3 wnormal;\n"
        "    vec3 wtangent;\n"
        "    vec3 wbitangent;\n"
        "} vs_out;\n"
        "\n";
    }

    std::string Mesh::shaderUniformsGeometry() {
        return
        "uniform mat3 u_mat_normal;\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "uniform vec3 u_camera_position;\n"
        "\n"
        "in VS_OUT {\n"
        "    vec4 position;\n"
        "    vec3 wposition;\n"
        "    vec2 uv;\n"
        "    vec3 wnormal;\n"
        "    vec3 wtangent;\n"
        "    vec3 wbitangent;\n"
        "} gs_in[];\n"
        "\n"
        "out VS_OUT {\n"
        "    vec4 position;\n"
        "    vec3 wposition;\n"
        "    vec2 uv;\n"
        "    vec3 wnormal;\n"
        "    vec3 wtangent;\n"
        "    vec3 wbitangent;\n"
        "} gs_out;\n"
        "\n";
    }

    std::string Mesh::shaderUniformsFragment() {
        return
        "in VS_OUT {\n"
        "    vec4 position;\n"
        "    vec3 wposition;\n"
        "    vec2 uv;\n"
        "    vec3 wnormal;\n"
        "    vec3 wtangent;\n"
        "    vec3 wbitangent;\n"
        "} fs_in;\n"
        "\n"
        "struct Light {\n"
        "    int type;\n" // unused = 0; sunlight = 1; spotlight = 2; pointlight = 3; ambiantlight = 4;
        "    vec3 position;\n"
        "    vec3 color;\n"
        "    vec3 specular;\n"
        "    float intensity;\n"
        "    vec3 attenuation;\n"
        "    vec3 direction;\n"
        "    float angle;\n"
        "    float cone_attenuation;\n"
        "    int caster_index;\n" // -1 is no cast
        "};\n"
        "\n"
        "struct LightCaster {\n"
        "    mat4 vp;\n"
        "    sampler2D depth_map;\n"
        "};\n"
        "\n"
        "struct Material {\n"
        "    vec3 diffuse;\n"
        "    vec3 specular;\n"
        "    float roughness;\n"
        "    float metalness;\n"
        "    vec3 occlusion;\n"
        "    float shininess;\n"
        "};\n"
        "\n"
        // sent by mesh material :
        "uniform sampler2D texture_diffuse;\n"    // 1
        "uniform sampler2D texture_specular;\n"   // 2
        "uniform sampler2D texture_roughness;\n"  // 4
        "uniform sampler2D texture_metalness;\n"  // 8
        "uniform sampler2D texture_occlusion;\n"  // 16
        "uniform sampler2D texture_shininess;\n"  // 32
        "uniform sampler2D texture_normal;\n"     // 64
        "uniform sampler2D texture_mask;\n"       // 128
        "uniform int texturebitmask;\n"
        "uniform mat3 diffuse_transform;\n"
        "uniform mat3 specular_transform;\n"
        "uniform mat3 roughness_transform;\n"
        "uniform mat3 metalness_transform;\n"
        "uniform mat3 occlusion_transform;\n"
        "uniform mat3 shininess_transform;\n"
        "uniform mat3 normal_transform;\n"
        "uniform mat3 mask_transform;\n"
        "uniform Material material;\n"
        "\n"
        // sent by scene :
        "uniform Light lights[16];\n"
        "uniform int lights_count;\n"
        "uniform vec3 u_camera_position;\n"
        #ifdef SCHOOL_DUMPS
        "uniform mat4 light_casters_vp[4];\n"
        "uniform sampler2D light_casters_depth_map[4];\n"
        #else
        "uniform LightCaster light_casters[4];\n"
        #endif
        "\n"
        "out vec4 FragColor;\n"
        "\n";
    }

}
