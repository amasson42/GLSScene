//
//  GLSStandardShaders.cpp
//  GLScene
//
//  Created by Arthur MASSON on 4/27/18.
//  Copyright © 2018 Arthur MASSON. All rights reserved.
//

#include "GLSShader.hpp"
#include <iostream>

namespace GLS {

    std::shared_ptr<Shader> Shader::standardVertexMesh() {
        std::string src =
        "#version 410 core\n"

        "layout (location = 0) in vec3 vin_position;\n"
        "layout (location = 1) in vec3 vin_normal;\n"
        "layout (location = 2) in vec3 vin_tangent;\n"
        "layout (location = 3) in vec3 vin_bitangent;\n"
        "layout (location = 4) in vec2 vin_uv;\n"
        
        "out vec3 fin_position;\n"
        "out vec3 fin_wposition;\n"
        "out vec2 fin_uv;\n"
        "out vec3 fin_wnormal;\n"
        "out vec3 fin_wtangent;\n"
        "out vec3 fin_wbitangent;\n"
        
        "uniform mat3 u_mat_normal;\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        
        "void main()\n"
        "{\n"
        "    gl_Position = u_mat_projection * u_mat_view * u_mat_model * vec4(vin_position, 1.0);\n"
        "    fin_position = gl_Position.xyz;\n"
        "    fin_wposition = vec3(u_mat_model * vec4(vin_position, 1.0));\n"
        "    fin_wnormal = u_mat_normal * vin_normal;\n"
        "    fin_wtangent = u_mat_normal * vin_tangent;\n"
        "    fin_wbitangent = u_mat_normal * vin_bitangent;\n"
        "    fin_uv = vin_uv;\n"
        "}\n";
        return std::make_shared<Shader>(src, GL_VERTEX_SHADER);
    }
    
    std::shared_ptr<Shader> Shader::standardFragmentMesh() {
        std::string src =
        "#version 410 core\n\n"

        "out vec4 FragColor;\n"
        
        "in vec3 fin_position;\n"
        "in vec3 fin_wposition;\n"
        "in vec2 fin_uv;\n"
        "in vec3 fin_wnormal;\n"
        "in vec3 fin_wtangent;\n"
        "in vec3 fin_wbitangent;\n"
        
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
        "};\n"

        "struct Material {\n"
        "    vec3 diffuse;\n"
        "    vec3 specular;\n"
        "    float roughness;\n"
        "    float metalness;\n"
        "    vec3 occlusion;\n"
        "    float shininess;\n"
        "};\n"

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

        "uniform Light lights[16];\n"
        "uniform int lights_count;\n"
        "uniform vec3 camera_position;\n"

        "void main() {\n"

        "    if ((texturebitmask & 128) != 0) {\n"
        "       if (texture(texture_mask, (mask_transform * vec3(fin_uv, 1.0)).xy).a == 0.0)\n"
        "           discard;\n"
        "    }\n"
        "    vec3 diffuse       = (texturebitmask & 1) != 0 ? texture(texture_diffuse, (diffuse_transform * vec3(fin_uv, 1.0)).xy).xyz : material.diffuse;\n"
        "    vec3 specular      = (texturebitmask & 2) != 0 ? texture(texture_specular, (specular_transform * vec3(fin_uv, 1.0)).xy).xyz : material.specular;\n"
        "    float roughness    = (texturebitmask & 4) != 0 ? texture(texture_roughness, (roughness_transform * vec3(fin_uv, 1.0)).xy).x : material.roughness;\n"
        "    float metalness    = (texturebitmask & 8) != 0 ? texture(texture_metalness, (metalness_transform * vec3(fin_uv, 1.0)).xy).x : material.metalness;\n"
        "    vec3 occlusion     = (texturebitmask & 16) != 0 ? texture(texture_occlusion, (occlusion_transform * vec3(fin_uv, 1.0)).xy).xyz : material.occlusion;\n"
        "    float shininess    = (texturebitmask & 32) != 0 ? texture(texture_shininess, (shininess_transform * vec3(fin_uv, 1.0)).xy).x : material.shininess;\n"

        "    vec3 normal_value = fin_wnormal;\n"
        "    if ((texturebitmask & 64) != 0) {\n"
        "        normal_value = texture(texture_normal, (normal_transform * vec3(fin_uv, 1.0)).xy).xyz;\n"
        "        normal_value = normal_value * 2 - 1;\n"
        "    }\n"
        "    normal_value = normalize(normal_value);\n"

        "    vec3 normal_direction = fin_wnormal * normal_value.z + fin_wtangent * normal_value.x + fin_wbitangent * normal_value.y;\n"
        "    normal_direction = normalize(normal_direction);\n"

        "    vec3 diffuse_color = vec3(0);\n"
        "    vec3 ambient_color = vec3(0);\n"
        "    vec3 specular_color = vec3(0);\n"
        "    vec3 fcamera_position = normalize(fin_wposition - camera_position);\n"

        "    vec3 light_direction;\n"
        "    float diffuse_intensity;\n"
        "    float d;\n"

        "    int i;\n"
        "    for (i = 0; i < lights_count; i++) {\n"
        "        Light light = lights[i];\n"
        "        switch (light.type) {\n" // unused = 0; sunlight = 1; spotlight = 2; pointlight = 3; ambiantlight = 4;
        "           case 1:\n" // sunlight
        "               light_direction = normalize(light.direction);\n"
        "               diffuse_intensity = clamp(dot(light_direction, normal_direction), 0.0, 1.0);\n"
        "               diffuse_color += light.color * diffuse * diffuse_intensity;\n"
        "               if (diffuse_intensity > 0) {\n"
        "                   vec3 reflection = reflect(light_direction, normal_direction);\n"
        "                   float specular_intensity = pow(clamp(dot(reflection, fcamera_position), 0.0, 1.0), shininess);\n"
        "               }\n"
        "               break;"
        "           case 2:\n" // spotlight
        "               d = distance(light.position, fin_wposition);\n"
        "               light_direction = normalize(light.position - fin_wposition);\n"
        "               vec3 cone_direction = normalize(-light.direction);\n"
        "               float spot_result = dot(light_direction, cone_direction);\n"
        "               if (spot_result > cos(light.angle)) {\n"
        "                   float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * d + light.attenuation.z * d * d);\n"
        "                   attenuation *= pow(spot_result, light.cone_attenuation);\n"
        "                   diffuse_intensity = clamp(dot(light_direction, normal_direction), 0.0, 1.0);\n"
        "                   vec3 color = light.color * diffuse * diffuse_intensity;\n"
        "                   color *= attenuation;\n"
        "                   diffuse_color += color;\n"
        "               }\n"
        "               break;\n"
        "           case 3:\n" // pointlight
        "               d = distance(light.position, fin_wposition);\n"
        "               light_direction = normalize(light.position - fin_wposition);\n"
        "               float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * d + light.attenuation.z * d * d);\n"
        "               diffuse_intensity = clamp(dot(light_direction, normal_direction), 0.0, 1.0);\n"
        "               vec3 color = light.color * diffuse * diffuse_intensity;\n"
        "               color *= attenuation;\n"
        "               diffuse_color += color;\n"
        "               break;\n"
        "           case 4:\n"
        "               ambient_color += light.color * light.intensity;\n"
        "               break;\n"
        "        }\n"
        "    }\n"
        "    vec3 color = diffuse_color + ambient_color + specular_color;\n"
        "    FragColor = vec4(color, 1.0);\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }

    std::shared_ptr<ShaderProgram> ShaderProgram::_standardShaderProgram = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::standardShaderProgramMesh() {
        if (_standardShaderProgram == nullptr) {
            std::shared_ptr<Shader> vertex = Shader::standardVertexMesh();
            std::shared_ptr<Shader> fragment = Shader::standardFragmentMesh();
            _standardShaderProgram = std::make_shared<ShaderProgram>(*vertex, *fragment);
        }
        return _standardShaderProgram;
    }

}
