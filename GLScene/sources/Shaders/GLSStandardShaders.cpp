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

    //  Mesh

    std::shared_ptr<Shader> Shader::standardVertexMesh() {
        std::string src =
        "#version 400 core\n"

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
        
        // sent by scene:
        "uniform mat3 u_mat_normal;\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"

        // sent by node:
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
        "#version 400 core\n\n"

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

        // sent by scene :
        "uniform Light lights[16];\n"
        "uniform int lights_count;\n"
        "uniform vec3 u_camera_position;\n"

        // "    float near = 0.1;"
        // "    float far = 15.0;"
        // "    float z = gl_FragCoord.z * 2.0 - 1.0;"
        // "    float linearDepth = (2.0 * near * far) / (far + near - z * (far - near)) / far;"

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

        "    vec3 normal_value = vec3(0, 0, 1);\n"
        "    if ((texturebitmask & 64) != 0) {\n"
        "        normal_value = texture(texture_normal, (normal_transform * vec3(fin_uv, 1.0)).xy).xyz;\n"
        "        normal_value = normal_value * 2 - 1;\n"
        "        normal_value = normalize(normal_value);\n"
        "    }\n"

        "    vec3 normal_direction = fin_wnormal * normal_value.z + fin_wtangent * normal_value.x + fin_wbitangent * normal_value.y;\n"
        "    normal_direction = normalize(normal_direction);\n"

        "    vec3 diffuse_color = vec3(0);\n"
        "    vec3 ambient_color = vec3(0);\n"
        "    vec3 specular_color = vec3(0);\n"
        "    vec3 fcamera_position = normalize(u_camera_position - fin_wposition);\n"

        "    vec3 light_direction;\n"
        "    float diffuse_intensity;\n"
        "    float d;\n"
        "    float attenuation;\n"

        // TODO: refactorisation of light calculations
        "    int i;\n"
        "    for (i = 0; i < lights_count; i++) {\n"
        "        Light light = lights[i];\n"
        "        d = distance(light.position, fin_wposition);\n"
        "        attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * d + light.attenuation.z * d * d);\n"
        "        switch (light.type) {\n" // directional = 0; sunlight = 1; spotlight = 2; pointlight = 3; ambiantlight = 4;
        "           case 1:\n" // directional
        "               light_direction = normalize(-light.direction);\n"
        "               diffuse_intensity = clamp(dot(light_direction, normal_direction), 0.0, 1.0);\n"
        "               diffuse_color += light.color * diffuse * diffuse_intensity * attenuation;\n"
        "               if (diffuse_intensity > 0) {\n"
        "                   vec3 reflection = reflect(-light_direction, normal_direction);\n"
        "                   vec3 halfway_direction = normalize(fcamera_position + light_direction);\n"
        "                   float specular_intensity = pow(clamp(dot(normal_direction, halfway_direction), 0.0, 1.0), shininess);\n"
        "                   specular_color += diffuse_intensity * light.specular * specular * specular_intensity;\n"
        "               }\n"
        "               break;\n"
        "           case 2:\n" // spotlight
        "               light_direction = normalize(light.position - fin_wposition);\n"
        "               float cosangle = cos(light.angle);\n"
        "               float spot_result = dot(-light_direction, light.direction);\n"
        "               if (spot_result > cosangle) {\n"
        "                   float cosat = cos(light.angle - light.cone_attenuation);\n"
        "                   attenuation *= clamp((spot_result - cosangle) / (cosat - cosangle), 0.0, 1.0);\n"
        "                   diffuse_intensity = clamp(dot(light_direction, normal_direction), 0.0, 1.0);\n"
        "                   diffuse_color += light.color * diffuse * diffuse_intensity * attenuation;\n"
        "                   if (diffuse_intensity > 0) {\n"
        "                       vec3 reflection = reflect(-light_direction, normal_direction);\n"
        "                       vec3 halfway_direction = normalize(fcamera_position + light_direction);\n"
        "                       float specular_intensity = pow(clamp(dot(normal_direction, halfway_direction), 0.0, 1.0), shininess);\n"
        "                       specular_color += diffuse_intensity * light.specular * specular * specular_intensity;\n"
        "                   }\n"
        "               }\n"
        "               break;\n"
        "           case 3:\n" // pointlight
        "               light_direction = normalize(light.position - fin_wposition);\n"
        "               diffuse_intensity = clamp(dot(light_direction, normal_direction), 0.0, 1.0);\n"
        "               diffuse_color += light.color * diffuse * diffuse_intensity * attenuation;\n"
        "               if (diffuse_intensity > 0) {\n"
        "                   vec3 reflection = reflect(-light_direction, normal_direction);\n"
        "                   vec3 halfway_direction = normalize(fcamera_position + light_direction);\n"
        "                   float specular_intensity = pow(clamp(dot(normal_direction, halfway_direction), 0.0, 1.0), shininess);\n"
        "                   specular_color += diffuse_intensity * light.specular * specular * specular_intensity;\n"
        "               }\n"
        "               break;\n"
        "           case 4:\n" // ambiant
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

    std::shared_ptr<Shader> Shader::standardVertexInstancedMesh() {
        std::string src =
        "#version 400 core\n"

        "layout (location = 0) in vec3 vin_position;\n"
        "layout (location = 1) in vec3 vin_normal;\n"
        "layout (location = 2) in vec3 vin_tangent;\n"
        "layout (location = 3) in vec3 vin_bitangent;\n"
        "layout (location = 4) in vec2 vin_uv;\n"
        "layout (location = 5) in mat4 vin_model_matrix;\n"
        
        "out vec3 fin_position;\n"
        "out vec3 fin_wposition;\n"
        "out vec2 fin_uv;\n"
        "out vec3 fin_wnormal;\n"
        "out vec3 fin_wtangent;\n"
        "out vec3 fin_wbitangent;\n"
        
        // sent by scene:
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"

        // sent by node:
        "uniform mat4 u_mat_model;\n"
        
        "void main()\n"
        "{\n"
        "    mat4 model = u_mat_model * vin_model_matrix;\n"
        "    mat3 normal = transpose(inverse(mat3(model)));\n"
        "    gl_Position = u_mat_projection * u_mat_view * model * vec4(vin_position, 1.0);\n"
        "    fin_position = gl_Position.xyz;\n"
        "    fin_wposition = vec3(model * vec4(vin_position, 1.0));\n"
        "    fin_wnormal = normal * vin_normal;\n"
        "    fin_wtangent = normal * vin_tangent;\n"
        "    fin_wbitangent = normal * vin_bitangent;\n"
        "    fin_uv = vin_uv;\n"
        "}\n";
        return std::make_shared<Shader>(src, GL_VERTEX_SHADER);
    }
    
    std::shared_ptr<Shader> Shader::standardFragmentMeshSimpleColor() {
        std::string src =
        "#version 400 core\n\n"
        "out vec4 FragColor;\n"
        "uniform vec3 color;\n"
        "void main() {\n"
        "   FragColor = vec4(color, 1.0);\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }

    // ScreenTexture

    std::shared_ptr<Shader> Shader::standardVertexScreenTexture() {
        std::string src =
        "#version 400 core\n\n"
        "layout (location = 0) in vec2 vin_position;\n"
        "out vec2 fin_uv;\n"
        "void main() {\n"
        "   gl_Position = vec4(vin_position.x, vin_position.y, 0, 1);\n"
        "   fin_uv = (vin_position + vec2(1.0)) / 2.0;\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_VERTEX_SHADER);
    }

    std::shared_ptr<Shader> Shader::standardFragmentScreenTexture() {
        std::string src =
        "#version 400 core\n\n"
        "out vec4 FragColor;\n"
        "in vec2 fin_uv;\n"
        "uniform sampler2D screen_texture;\n"

        // Normal
        "void main() {\n"
        "   FragColor = texture(screen_texture, fin_uv);\n"
        "}\n"

        // Grayscale
        // "void main() {\n"
        // "   FragColor = texture(screen_texture, fin_uv);\n"
        // "   float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;\n"
        // "   FragColor = vec4(vec3(average), 1.0);\n"
        // "}\n"

        // Kernel effect
        // "void main() {"
        // "   const float offset = 1.0 / 300.0;"
        // "   vec2 offsets[9] = vec2[]("
        // "       vec2(-offset,  offset)," // top-left
        // "       vec2( 0.0f,    offset)," // top-center
        // "       vec2( offset,  offset)," // top-right
        // "       vec2(-offset,  0.0f),  " // center-left
        // "       vec2( 0.0f,    0.0f),  " // center-center
        // "       vec2( offset,  0.0f),  " // center-right
        // "       vec2(-offset, -offset)," // bottom-left
        // "       vec2( 0.0f,   -offset)," // bottom-center
        // "       vec2( offset, -offset) " // bottom-right    
        // "   );"

        // "   float kernel[9] = float[](" // Simple
        // "       -1, -1, -1,"
        // "       -1,  9, -1,"
        // "       -1, -1, -1"
        // "   );"

        // // "   float kernel[9] = float[](" // Edge
        // // "       1, 1, 1,"
        // // "       1, -8, 1,"
        // // "       1, 1, 1"
        // // "   );"

        // // "   float kernel[9] = float[](" // Blur
        // // "       1.0 / 16, 2.0 / 16, 1.0 / 16,"
        // // "       2.0 / 16, 4.0 / 16, 2.0 / 16,"
        // // "       1.0 / 16, 2.0 / 16, 1.0 / 16"
        // // "   );"

        // "   vec3 sampleTex[9];"
        // "   for(int i = 0; i < 9; i++)"
        // "       sampleTex[i] = vec3(texture(screen_texture, fin_uv.st + offsets[i]));"
        // "   vec3 col = vec3(0.0);"
        // "   for(int i = 0; i < 9; i++)"
        // "   col += sampleTex[i] * kernel[i];"
        // "   FragColor = vec4(col, 1.0);"
        // "}" 

        "\n";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }

    // Skybox

    std::shared_ptr<Shader> Shader::standardVertexSkybox() {
        std::string src =
        "#version 400 core\n\n"

        "layout (location = 0) in vec3 vin_position;\n"

        "out vec3 fin_uv;\n"

        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"

        "void main() {\n"
        "   fin_uv = vin_position;\n"
        "   vec4 pos = u_mat_projection * u_mat_view * vec4(vin_position, 1.0);\n"
        "   gl_Position = pos.xyzw;\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_VERTEX_SHADER);
    }

    std::shared_ptr<Shader> Shader::standardFragmentSkybox() {
        std::string src =
        "#version 400 core\n\n"

        "out vec4 FragColor;\n"

        "in vec3 fin_uv;\n"

        "uniform samplerCube u_skybox;\n"

        "void main() {\n"
        "   FragColor = texture(u_skybox, normalize(fin_uv));\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }

}
