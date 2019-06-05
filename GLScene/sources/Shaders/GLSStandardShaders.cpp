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

        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec3 normal;\n"
        "layout (location = 2) in vec3 tangent;\n"
        "layout (location = 3) in vec3 bitangent;\n"
        "layout (location = 4) in vec2 uv;\n"

        "out VS_OUT {\n"
        "    vec4 position;\n"
        "    vec3 wposition;\n"
        "    vec2 uv;\n"
        "    vec3 wnormal;\n"
        "    vec3 wtangent;\n"
        "    vec3 wbitangent;\n"
        "} vs_out;\n"
        
        // sent by scene:
        "uniform mat3 u_mat_normal;\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"

        // sent by node:
        "uniform mat4 u_mat_model;\n"
        
        "void main()\n"
        "{\n"
        "    vs_out.wposition = vec3(u_mat_model * vec4(position, 1.0));\n"
        "    gl_Position = u_mat_projection * u_mat_view * vec4(vs_out.wposition, 1.0);\n"
        "    vs_out.position = gl_Position;\n"
        "    vs_out.wnormal = u_mat_normal * normal;\n"
        "    vs_out.wtangent = u_mat_normal * tangent;\n"
        "    vs_out.wbitangent = u_mat_normal * bitangent;\n"
        "    vs_out.uv = uv;\n"
        "}\n";
        return std::make_shared<Shader>(src, GL_VERTEX_SHADER);
    }

    std::shared_ptr<Shader> Shader::standardFragmentMesh() {
        std::string src =
        "#version 400 core\n\n"

        "out vec4 FragColor;\n"

        "in VS_OUT {\n"
        "    vec4 position;\n"
        "    vec3 wposition;\n"
        "    vec2 uv;\n"
        "    vec3 wnormal;\n"
        "    vec3 wtangent;\n"
        "    vec3 wbitangent;\n"
        "} fs_in;\n"
        
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
        "    int caster_index;\n" // if the index >= 0 then there is a light caster bounds
        "};\n"

        "struct LightCaster {\n"
        "    mat4 vp;\n"
        "    sampler2D depth_map;\n"
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

        #ifdef SCHOOL_DUMPS
        "uniform mat4 light_casters_vp[4];\n"
        "uniform sampler2D light_casters_depth_map[4];\n"
        #else
        "uniform LightCaster light_casters[4];\n"
        #endif

        // "    float near = 0.1;"
        // "    float far = 15.0;"
        // "    float z = gl_FragCoord.z * 2.0 - 1.0;"
        // "    float linearDepth = (2.0 * near * far) / (far + near - z * (far - near)) / far;"

        // fixed but the workaround is an ugly code
        #ifdef SCHOOL_DUMPS
        "float calculShadow(Light light) {\n"
        "    int caster_index = light.caster_index;\n"
        "    if (caster_index >= 0) {\n"
        "        vec4 fpos_light = light_casters_vp[caster_index] * vec4(fs_in.wposition, 1.0);\n"
        "        vec3 proj_coords = fpos_light.xyz / fpos_light.w;\n"
        "        proj_coords = proj_coords * 0.5 + 0.5;\n"
        "        float current_depth = proj_coords.z;\n"
        "        float bias = 0.0005;\n"// max(0.05 * (1.0 - dot(normal_direction, -light.direction)), 0.005);\n"
        "        vec2 texel_size = 1.0 / textureSize(light_casters_depth_map[caster_index], 0);\n"
        "        float shadow = 0.0;\n"
        "        for (int x = -1; x <= 1; x++)\n"
        "            for (int y = -1; y <= 1; y++) {\n"
        "                float pcf_depth = texture(light_casters_depth_map[caster_index], proj_coords.xy + vec2(x, y) * texel_size).r;\n"
        "                shadow += current_depth - bias > pcf_depth ? 0.0 : 1.0;\n"
        "            }\n"
        "        return shadow / 9.0;\n"
        "    }\n"
        "    return 1.0;\n"
        "}\n"
        #else
        "float calculShadow(Light light) {\n"
        "    int caster_index = light.caster_index;\n"
        "    if (caster_index >= 0) {\n"
        "        vec4 fpos_light = light_casters[caster_index].vp * vec4(fs_in.wposition, 1.0);\n"
        "        vec3 proj_coords = fpos_light.xyz / fpos_light.w;\n"
        "        proj_coords = proj_coords * 0.5 + 0.5;\n"
        "        float current_depth = proj_coords.z;\n"
        "        float bias = 0.0005;\n"// max(0.05 * (1.0 - dot(normal_direction, -light.direction)), 0.005);\n"
        "        vec2 texel_size = 1.0 / textureSize(light_casters[caster_index].depth_map, 0);\n"
        "        float shadow = 0.0;\n"
        "        for (int x = -1; x <= 1; x++)\n"
        "            for (int y = -1; y <= 1; y++) {\n"
        "                float pcf_depth = texture(light_casters[caster_index].depth_map, proj_coords.xy + vec2(x, y) * texel_size).r;\n"
        "                shadow += current_depth - bias > pcf_depth ? 0.0 : 1.0;\n"
        "            }\n"
        "        return shadow / 9.0;\n"
        "    }\n"
        "    return 1.0;\n"
        "}\n"
        #endif

        "vec3 calculDirectionalLight(Light light, Material fragMat, vec3 normal_direction, vec3 fcamera_position) {\n"
        "    vec3 color = vec3(0);\n"
        "    float shadow = calculShadow(light);\n"
        "    if (shadow <= 0.0) return color;\n"
        "    float d = distance(light.position, fs_in.wposition);\n"
        "    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * d + light.attenuation.z * d * d);\n"
        "    vec3 light_direction = normalize(-light.direction);\n"
        "    float diffuse_intensity = clamp(dot(light_direction, normal_direction), 0.0, 1.0);\n"
        "    color += light.color * fragMat.diffuse * diffuse_intensity * attenuation * shadow * fragMat.occlusion;\n"
        "    if (diffuse_intensity > 0) {\n"
        "        vec3 reflection = reflect(-light_direction, normal_direction);\n"
        "        vec3 halfway_direction = normalize(fcamera_position + light_direction);\n"
        "        float specular_intensity = pow(clamp(dot(normal_direction, halfway_direction), 0.0, 1.0), fragMat.shininess);\n"
        "        color += diffuse_intensity * light.specular * fragMat.specular * specular_intensity * shadow;\n"
        "    }\n"
        "    return color;\n"
        "}\n"

        "vec3 calculSpotLight(Light light, Material fragMat, vec3 normal_direction, vec3 fcamera_position) {\n"
        "    vec3 color = vec3(0);\n"
        "    float shadow = calculShadow(light);\n"
        "    if (shadow <= 0.0) return color;\n"
        "    float d = distance(light.position, fs_in.wposition);\n"
        "    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * d + light.attenuation.z * d * d);\n"
        "    vec3 light_direction = normalize(light.position - fs_in.wposition);\n"
        "    float cosangle = cos(light.angle);\n"
        "    float spot_result = dot(-light_direction, light.direction);\n"
        "    if (spot_result > cosangle) {\n"
        "        float cosat = cos(light.angle - light.cone_attenuation);\n"
        "        attenuation *= clamp((spot_result - cosangle) / (cosat - cosangle), 0.0, 1.0);\n"
        "        float diffuse_intensity = clamp(dot(light_direction, normal_direction), 0.0, 1.0);\n"
        "        color += light.color * fragMat.diffuse * diffuse_intensity * attenuation * shadow * fragMat.occlusion;\n"
        "        if (diffuse_intensity > 0) {\n"
        "            vec3 reflection = reflect(-light_direction, normal_direction);\n"
        "            vec3 halfway_direction = normalize(fcamera_position + light_direction);\n"
        "            float specular_intensity = pow(clamp(dot(normal_direction, halfway_direction), 0.0, 1.0), fragMat.shininess);\n"
        "            color += diffuse_intensity * light.specular * fragMat.specular * specular_intensity * shadow;\n"
        "        }\n"
        "    }\n"
        "    return color;\n"
        "}\n"

        "vec3 calculPointLight(Light light, Material fragMat, vec3 normal_direction, vec3 fcamera_position) {\n"
        "    vec3 color = vec3(0);\n"
        "    float shadow = calculShadow(light);\n"
        "    if (shadow <= 0.0) return color;\n"
        "    float d = distance(light.position, fs_in.wposition);\n"
        "    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * d + light.attenuation.z * d * d);\n"
        "    vec3 light_direction = normalize(light.position - fs_in.wposition);\n"
        "    float diffuse_intensity = clamp(dot(light_direction, normal_direction), 0.0, 1.0);\n"
        "    color += light.color * fragMat.diffuse * diffuse_intensity * attenuation * shadow * fragMat.occlusion;\n"
        "    if (diffuse_intensity > 0) {\n"
        "        vec3 reflection = reflect(-light_direction, normal_direction);\n"
        "        vec3 halfway_direction = normalize(fcamera_position + light_direction);\n"
        "        float specular_intensity = pow(clamp(dot(normal_direction, halfway_direction), 0.0, 1.0), fragMat.shininess);\n"
        "        color += diffuse_intensity * light.specular * fragMat.specular * specular_intensity * shadow;\n"
        "    }\n"
        "    return color;\n"
        "}\n"

        "vec3 calculAmbiantLight(Light light, Material fragMat) {\n"
        "    return fragMat.diffuse * light.color * light.intensity * fragMat.occlusion;\n"
        "}\n"

        "vec3 calculLight(Light light, Material fragMat, vec3 normal_direction, vec3 fcamera_position) {\n"
        "    switch (light.type) {\n" // directional = 1; spotlight = 2; pointlight = 3; ambiantlight = 4;
        "       case 1:\n" // directional
        "           return calculDirectionalLight(light, fragMat, normal_direction, fcamera_position);\n"
        "       case 2:\n" // spotlight
        "           return calculSpotLight(light, fragMat, normal_direction, fcamera_position);\n"
        "       case 3:\n" // pointlight
        "           return calculPointLight(light, fragMat, normal_direction, fcamera_position);\n"
        "       case 4:\n" // ambiant
        "           return calculAmbiantLight(light, fragMat);\n"
        "       default:\n"
        "           return vec3(0);\n"
        "    }\n"
        "}\n"

        "void main() {\n"
        "    if ((texturebitmask & 128) != 0) {\n"
        "       if (texture(texture_mask, (mask_transform * vec3(fs_in.uv, 1.0)).xy).a == 0.0)\n"
        "           discard;\n"
        "    }\n"
        "    Material fragMat;"
        "    fragMat.diffuse    = (texturebitmask & 1) != 0 ? texture(texture_diffuse, (diffuse_transform * vec3(fs_in.uv, 1.0)).xy).xyz : material.diffuse;\n"
        "    fragMat.specular   = (texturebitmask & 2) != 0 ? texture(texture_specular, (specular_transform * vec3(fs_in.uv, 1.0)).xy).xyz : material.specular;\n"
        "    fragMat.roughness  = (texturebitmask & 4) != 0 ? texture(texture_roughness, (roughness_transform * vec3(fs_in.uv, 1.0)).xy).x : material.roughness;\n"
        "    fragMat.metalness  = (texturebitmask & 8) != 0 ? texture(texture_metalness, (metalness_transform * vec3(fs_in.uv, 1.0)).xy).x : material.metalness;\n"
        "    fragMat.occlusion  = (texturebitmask & 16) != 0 ? texture(texture_occlusion, (occlusion_transform * vec3(fs_in.uv, 1.0)).xy).xyz : material.occlusion;\n"
        "    fragMat.shininess  = (texturebitmask & 32) != 0 ? texture(texture_shininess, (shininess_transform * vec3(fs_in.uv, 1.0)).xy).x : material.shininess;\n"

        "    vec3 normal_value = vec3(0, 0, 1);\n"
        "    if ((texturebitmask & 64) != 0) {\n"
        "        normal_value = texture(texture_normal, (normal_transform * vec3(fs_in.uv, 1.0)).xy).xyz;\n"
        "        normal_value = normal_value * 2 - 1;\n"
        "        normal_value = normalize(normal_value);\n"
        "    }\n"

        "    vec3 normal_direction = fs_in.wnormal * normal_value.z + fs_in.wtangent * normal_value.x + fs_in.wbitangent * normal_value.y;\n"
        "    normal_direction = normalize(normal_direction);\n"
        "    vec3 fcamera_position = normalize(u_camera_position - fs_in.wposition);\n"

        "    vec3 color = vec3(0);\n"

        "    for (int i = 0; i < lights_count; i++) {\n"
        "        color += calculLight(lights[i], fragMat, normal_direction, fcamera_position);\n"
        "    }\n"
        "    FragColor = vec4(color, 1.0);\n"
        "}\n"

        "\n";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }

    std::shared_ptr<Shader> Shader::standardVertexInstancedMesh() {
        std::string src =
        "#version 400 core\n"

        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec3 normal;\n"
        "layout (location = 2) in vec3 tangent;\n"
        "layout (location = 3) in vec3 bitangent;\n"
        "layout (location = 4) in vec2 uv;\n"
        "layout (location = 5) in mat4 model_matrix;\n"
        
        "out VS_OUT {\n"
        "    vec4 position;\n"
        "    vec3 wposition;\n"
        "    vec2 uv;\n"
        "    vec3 wnormal;\n"
        "    vec3 wtangent;\n"
        "    vec3 wbitangent;\n"
        "} vs_out;\n"
        
        // sent by scene:
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"

        // sent by node:
        "uniform mat4 u_mat_model;\n"
        
        "void main()\n"
        "{\n"
        "    mat4 model = u_mat_model * model_matrix;\n"
        "    mat3 mat_normal = transpose(inverse(mat3(model)));\n"
        "    gl_Position = u_mat_projection * u_mat_view * model * vec4(position, 1.0);\n"
        "    vs_out.position = gl_Position;\n"
        "    vs_out.wposition = vec3(model * vec4(position, 1.0));\n"
        "    vs_out.wnormal = mat_normal * normal;\n"
        "    vs_out.wtangent = mat_normal * tangent;\n"
        "    vs_out.wbitangent = mat_normal * bitangent;\n"
        "    vs_out.uv = uv;\n"
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

        "out VS_OUT {\n"
        "    vec2 uv;\n"
        "} vs_out;\n"

        "void main() {\n"
        "   gl_Position = vec4(vin_position.x, vin_position.y, 0, 1);\n"
        "   vs_out.uv = (vin_position + vec2(1.0)) / 2.0;\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_VERTEX_SHADER);
    }

    std::shared_ptr<Shader> Shader::standardFragmentScreenTexture() {
        std::string src =
        "#version 400 core\n\n"

        "out vec4 FragColor;\n"

        "in VS_OUT {\n"
        "    vec2 uv;\n"
        "} fs_in;\n"

        "uniform sampler2D screen_texture;\n"

        // Normal
        "void main() {\n"
        "   FragColor = texture(screen_texture, fs_in.uv);\n"
        "}\n"

        // Grayscale
        // "void main() {\n"
        // "   FragColor = texture(screen_texture, fs_in.uv);\n"
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
        // "       sampleTex[i] = vec3(texture(screen_texture, fs_in.uv.st + offsets[i]));"
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

        "out VS_OUT {\n"
        "    vec3 uv;\n"
        "} vs_out;\n"

        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"

        "void main() {\n"
        "   vs_out.uv = vin_position;\n"
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

        "in VS_OUT {\n"
        "    vec3 uv;\n"
        "} fs_in;\n"

        "uniform samplerCube u_skybox;\n"

        "void main() {\n"
        "   FragColor = texture(u_skybox, normalize(fs_in.uv));\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }

    // VoxelChunk

    std::shared_ptr<Shader> Shader::standardVertexVoxelChunk() {
        std::string src =
        "#version 400 core\n"

        "layout (location = 0) in int blockId;\n"

        "out GS_IN {"
        "    int position_x;\n"
        "    int position_y;\n"
        "    int position_z;\n"
        "    flat int blockAdj;\n"
        "    flat int blockId;\n"
        "} gs_in;\n"

        "const int chunkSize = " + std::to_string(VoxelChunk::chunkSize) + ";\n"

        "void main()\n"
        "{\n"
        "    gs_in.position_x = gl_VertexID % chunkSize;\n"
        "    gs_in.position_y = (gl_VertexID % (chunkSize * chunkSize)) / chunkSize;\n"
        "    gs_in.position_z = gl_VertexID / (chunkSize * chunkSize);\n"
        "    gs_in.blockAdj = (blockId & 0xFF0000) >> 16;\n"
        "    gs_in.blockId = blockId & 0xFFFF;\n"
        "}\n";
        return std::make_shared<Shader>(src, GL_VERTEX_SHADER);
    }

    std::shared_ptr<Shader> Shader::standardGeometryVoxelChunk() {
        std::string src =
        "#version 400 core\n\n"

        "layout (points) in;\n"
        "layout (triangle_strip, max_vertices = 24) out;\n"
        
        "in GS_IN {\n"
        "    int position_x;\n"
        "    int position_y;\n"
        "    int position_z;\n"
        "    flat int blockAdj;\n"
        "    flat int blockId;\n"
        "} gs_in[];\n"

        "out VS_OUT {\n"
        "    vec4 position;\n"
        "    vec3 wposition;\n"
        "    vec2 uv;\n"
        "    vec3 wnormal;\n"
        "    vec3 wtangent;\n"
        "    vec3 wbitangent;\n"
        "} gs_out;\n"

        // sent by scene:
        "uniform mat3 u_mat_normal;\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"

        // sent by node:
        "uniform mat4 u_mat_model;\n"

        "const vec2 uvs = vec2(0.0, 1);\n"

        // +X
        "void drawFace_positiveX_emitVertex(vec3 origin, vec3 axe_x, vec3 axe_y, vec3 axe_z, vec2 uv) {\n"
        "    gs_out.uv = uv;\n"
        "    gs_out.wposition = origin + axe_x + axe_y * gs_out.uv.y + axe_z * (1.0 - gs_out.uv.x);\n"
        "    gs_out.position = u_mat_projection * u_mat_view * vec4(gs_out.wposition, 1.0);\n"
        "    gs_out.wnormal = axe_x;\n"
        "    gs_out.wtangent = -axe_z;\n"
        "    gs_out.wbitangent = axe_y;\n"
        "    gl_Position = gs_out.position;\n"
        "    EmitVertex();\n"
        "}\n"

        "void drawFace_positiveX(vec3 origin, vec3 axe_x, vec3 axe_y, vec3 axe_z) {\n"
        "    drawFace_positiveX_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.x, uvs.x));\n"
        "    drawFace_positiveX_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.y, uvs.x));\n"
        "    drawFace_positiveX_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.x, uvs.y));\n"
        "    drawFace_positiveX_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.y, uvs.y));\n"
        "    EndPrimitive();\n"
        "}\n"

        // -X
        "void drawFace_negativeX_emitVertex(vec3 origin, vec3 axe_x, vec3 axe_y, vec3 axe_z, vec2 uv) {\n"
        "    gs_out.uv = uv;\n"
        "    gs_out.wposition = origin + axe_y * gs_out.uv.y + axe_z * gs_out.uv.x;\n"
        "    gs_out.position = u_mat_projection * u_mat_view * vec4(gs_out.wposition, 1.0);\n"
        "    gs_out.wnormal = -axe_x;\n"
        "    gs_out.wtangent = axe_z;\n"
        "    gs_out.wbitangent = axe_y;\n"
        "    gl_Position = gs_out.position;\n"
        "    EmitVertex();\n"
        "}\n"
    
        "void drawFace_negativeX(vec3 origin, vec3 axe_x, vec3 axe_y, vec3 axe_z) {\n"
        "    drawFace_negativeX_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.x, uvs.x));\n"
        "    drawFace_negativeX_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.y, uvs.x));\n"
        "    drawFace_negativeX_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.x, uvs.y));\n"
        "    drawFace_negativeX_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.y, uvs.y));\n"
        "    EndPrimitive();\n"
        "}\n"

        // +Y
        "void drawFace_positiveY_emitVertex(vec3 origin, vec3 axe_x, vec3 axe_y, vec3 axe_z, vec2 uv) {\n"
        "    gs_out.uv = uv;\n"
        "    gs_out.wposition = origin + axe_x * gs_out.uv.x + axe_y + axe_z * (1 - gs_out.uv.y);\n"
        "    gs_out.position = u_mat_projection * u_mat_view * vec4(gs_out.wposition, 1.0);\n"
        "    gs_out.wnormal = axe_y;\n"
        "    gs_out.wtangent = axe_x;\n"
        "    gs_out.wbitangent = -axe_z;\n"
        "    gl_Position = gs_out.position;\n"
        "    EmitVertex();\n"
        "}\n"

        "void drawFace_positiveY(vec3 origin, vec3 axe_x, vec3 axe_y, vec3 axe_z) {\n"
        "    drawFace_positiveY_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.x, uvs.x));\n"
        "    drawFace_positiveY_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.y, uvs.x));\n"
        "    drawFace_positiveY_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.x, uvs.y));\n"
        "    drawFace_positiveY_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.y, uvs.y));\n"
        "    EndPrimitive();\n"
        "}\n"

        // -Y
        "void drawFace_negativeY_emitVertex(vec3 origin, vec3 axe_x, vec3 axe_y, vec3 axe_z, vec2 uv) {\n"
        "    gs_out.uv = uv;\n"
        "    gs_out.wposition = origin + axe_x * (1 - gs_out.uv.x) + axe_z * (1 - gs_out.uv.y);\n"
        "    gs_out.position = u_mat_projection * u_mat_view * vec4(gs_out.wposition, 1.0);\n"
        "    gs_out.wnormal = -axe_y;\n"
        "    gs_out.wtangent = -axe_x;\n"
        "    gs_out.wbitangent = -axe_z;\n"
        "    gl_Position = gs_out.position;\n"
        "    EmitVertex();\n"
        "}\n"

        "void drawFace_negativeY(vec3 origin, vec3 axe_x, vec3 axe_y, vec3 axe_z) {\n"
        "    drawFace_negativeY_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.x, uvs.x));\n"
        "    drawFace_negativeY_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.y, uvs.x));\n"
        "    drawFace_negativeY_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.x, uvs.y));\n"
        "    drawFace_negativeY_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.y, uvs.y));\n"
        "    EndPrimitive();\n"
        "}\n"

        // +Z
        "void drawFace_positiveZ_emitVertex(vec3 origin, vec3 axe_x, vec3 axe_y, vec3 axe_z, vec2 uv) {\n"
        "    gs_out.uv = uv;\n"
        "    gs_out.wposition = origin + axe_x * gs_out.uv.x + axe_y * gs_out.uv.y + axe_z;\n"
        "    gs_out.position = u_mat_projection * u_mat_view * vec4(gs_out.wposition, 1.0);\n"
        "    gs_out.wnormal = axe_z;\n"
        "    gs_out.wtangent = axe_x;\n"
        "    gs_out.wbitangent = axe_y;\n"
        "    gl_Position = gs_out.position;\n"
        "    EmitVertex();\n"
        "}\n"

        "void drawFace_positiveZ(vec3 origin, vec3 axe_x, vec3 axe_y, vec3 axe_z) {\n"
        "    drawFace_positiveZ_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.x, uvs.x));\n"
        "    drawFace_positiveZ_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.y, uvs.x));\n"
        "    drawFace_positiveZ_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.x, uvs.y));\n"
        "    drawFace_positiveZ_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.y, uvs.y));\n"
        "    EndPrimitive();\n"
        "}\n"

        // -Z
        "void drawFace_negativeZ_emitVertex(vec3 origin, vec3 axe_x, vec3 axe_y, vec3 axe_z, vec2 uv) {\n"
        "    gs_out.uv = uv;\n"
        "    gs_out.wposition = origin + axe_x * (1 - gs_out.uv.x) + axe_y * gs_out.uv.y;\n"
        "    gs_out.position = u_mat_projection * u_mat_view * vec4(gs_out.wposition, 1.0);\n"
        "    gs_out.wnormal = -axe_z;\n"
        "    gs_out.wtangent = -axe_x;\n"
        "    gs_out.wbitangent = axe_y;\n"
        "    gl_Position = gs_out.position;\n"
        "    EmitVertex();\n"
        "}\n"

        "void drawFace_negativeZ(vec3 origin, vec3 axe_x, vec3 axe_y, vec3 axe_z) {\n"
        "    drawFace_negativeZ_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.x, uvs.x));\n"
        "    drawFace_negativeZ_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.y, uvs.x));\n"
        "    drawFace_negativeZ_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.x, uvs.y));\n"
        "    drawFace_negativeZ_emitVertex(origin, axe_x, axe_y, axe_z, vec2(uvs.y, uvs.y));\n"
        "    EndPrimitive();\n"
        "}\n"

        "void main() {\n"
        "    vec3 axe_x = vec3(u_mat_model * vec4(1, 0, 0, 0));\n"
        "    vec3 axe_y = vec3(u_mat_model * vec4(0, 1, 0, 0));\n"
        "    vec3 axe_z = vec3(u_mat_model * vec4(0, 0, 1, 0));\n"
        "    vec3 origin = vec3(u_mat_model * vec4(0, 0, 0, 1))"
        "        + axe_x * gs_in[0].position_x"
        "        + axe_y * gs_in[0].position_y"
        "        + axe_z * gs_in[0].position_z;\n"

        "    if (gs_in[0].blockId == 0)\n"
        "        return;"

        "    if ((gs_in[0].blockAdj & (1 << 0)) != 0)\n"
        "        drawFace_positiveX(origin, axe_x, axe_y, axe_z);\n"
        "    if ((gs_in[0].blockAdj & (1 << 1)) != 0)\n"
        "        drawFace_negativeX(origin, axe_x, axe_y, axe_z);\n"
        "    if ((gs_in[0].blockAdj & (1 << 2)) != 0)\n"
        "        drawFace_positiveY(origin, axe_x, axe_y, axe_z);\n"
        "    if ((gs_in[0].blockAdj & (1 << 3)) != 0)\n"
        "        drawFace_negativeY(origin, axe_x, axe_y, axe_z);\n"
        "    if ((gs_in[0].blockAdj & (1 << 4)) != 0)\n"
        "        drawFace_positiveZ(origin, axe_x, axe_y, axe_z);\n"
        "    if ((gs_in[0].blockAdj & (1 << 5)) != 0)\n"
        "        drawFace_negativeZ(origin, axe_x, axe_y, axe_z);\n"

        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_GEOMETRY_SHADER);
    }

}
