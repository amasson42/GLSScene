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

        + Mesh::shaderUniformsVertex() +
        
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

        + Mesh::shaderUniformsFragment() +
        // "    float near = 0.1;"
        // "    float far = 15.0;"
        // "    float z = gl_FragCoord.z * 2.0 - 1.0;"
        // "    float linearDepth = (2.0 * near * far) / (far + near - z * (far - near)) / far;"

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

        + InstancedMesh::shaderUniformsVertex() +
        
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

        "struct Material {\n"
        "    vec3 diffuse;\n"
        "};\n"

        "uniform Material material;\n"

        "out vec4 FragColor;\n"

        "void main() {\n"
        "    FragColor = vec4(material.diffuse, 1.0);\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }

    // ScreenTexture

    std::shared_ptr<Shader> Shader::standardVertexScreenTexture() {
        std::string src =
        "#version 400 core\n\n"

        + Framebuffer::shaderUniformsVertex() +

        "void main() {\n"
        "    gl_Position = vec4(position.x, position.y, 0, 1);\n"
        "    vs_out.uv = (position + vec2(1.0)) / 2.0;\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_VERTEX_SHADER);
    }

    std::shared_ptr<Shader> Shader::standardFragmentScreenTexture() {
        std::string src =
        "#version 400 core\n\n"

        + Framebuffer::shaderUniformsFragment() +

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

        + Skybox::shaderUniformsVertex() +

        "void main() {\n"
        "    vs_out.uv = position;\n"
        "    vec4 pos = u_mat_projection * u_mat_view * vec4(position, 1.0);\n"
        "    gl_Position = pos.xyzw;\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_VERTEX_SHADER);
    }

    std::shared_ptr<Shader> Shader::standardFragmentSkybox() {
        std::string src =
        "#version 400 core\n\n"

        + Skybox::shaderUniformsFragment() +

        "void main() {\n"
        "    FragColor = texture(u_skybox, normalize(fs_in.uv));\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }

    // VoxelChunk

    std::shared_ptr<Shader> Shader::standardVertexVoxelChunk() {
        std::string src =
        "#version 400 core\n"

        + VoxelChunk::shaderUniformsVertex() +

        "void main()\n"
        "{\n"
        "    vs_out.position_x = gl_VertexID % chunkSize;\n"
        "    vs_out.position_y = (gl_VertexID % (chunkSize * chunkSize)) / chunkSize;\n"
        "    vs_out.position_z = gl_VertexID / (chunkSize * chunkSize);\n"
        "    vs_out.blockAdj = (blockId & 0xFF0000) >> 16;\n"
        "    vs_out.blockId = blockId & 0xFFFF;\n"
        "}\n";
        return std::make_shared<Shader>(src, GL_VERTEX_SHADER);
    }

    std::shared_ptr<Shader> Shader::standardGeometryVoxelChunk() {
        std::string src =
        "#version 400 core\n\n"

        "layout (points) in;\n"
        "layout (triangle_strip, max_vertices = 24) out;\n"

        + VoxelChunk::shaderUniformsGeometry() +

        // "const vec2 uvs = vec2(0.05, 0.95);\n"
        "const vec2 uvs = vec2(0, 1);\n"

        "vec2 getBlockIdUvs(vec2 uv, int blockId, int face) {\n" // face: 0 = bot; 1 = side; 2 = top
        "    blockId--;\n"
        "    int gridX = (blockId % 10);\n"
        "    int gridY = (blockId / 10);\n"
        "    uv.x = ((gridX * 3) + face + uv.x) * (1.0 / 30.0);\n"
        "    uv.y = (gridY + (1 - uv.y)) * (1.0 / 10.0);\n"
        "    return uv;\n"
        "}\n"

        // +X
        "void drawFace_positiveX_emitVertex(vec3 origin, vec3 axe_x, vec3 axe_y, vec3 axe_z, vec2 uv) {\n"
        "    gs_out.uv = uv;\n"
        "    gs_out.wposition = origin + axe_x + axe_y * gs_out.uv.y + axe_z * (1.0 - gs_out.uv.x);\n"
        "    gs_out.position = u_mat_projection * u_mat_view * vec4(gs_out.wposition, 1.0);\n"
        "    gs_out.wnormal = axe_x;\n"
        "    gs_out.wtangent = -axe_z;\n"
        "    gs_out.wbitangent = axe_y;\n"
        "    gl_Position = gs_out.position;\n"
        "    gs_out.uv = getBlockIdUvs(uv, gs_in[0].blockId, 1);\n"
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
        "    gs_out.uv = getBlockIdUvs(uv, gs_in[0].blockId, 1);\n"
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
        "    gs_out.uv = getBlockIdUvs(uv, gs_in[0].blockId, 2);\n"
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
        "    gs_out.uv = getBlockIdUvs(uv, gs_in[0].blockId, 0);\n"
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
        "    gs_out.uv = getBlockIdUvs(uv, gs_in[0].blockId, 1);\n"
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
        "    gs_out.uv = getBlockIdUvs(uv, gs_in[0].blockId, 1);\n"
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

    std::shared_ptr<Shader> Shader::standardVertexParticleSystem() {
        std::string src =
        "#version 400 core\n\n"

        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec3 velocity;\n"
        "\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "\n"
        "out PARTICLE {\n"
        "    vec4 position;\n"
        "    vec2 uv;\n"
        "} vs_out;\n"
        "\n"

        "void main() {\n"
        "    vs_out.position = u_mat_projection * u_mat_view * u_mat_model * vec4(position, 1.0);\n"
        "    gl_Position = vs_out.position;\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_VERTEX_SHADER);
    }

    std::shared_ptr<Shader> Shader::standardGeometryTexturedParticleSystem() {
        std::string src =
        "#version 400 core\n\n"

        "layout (points) in;\n"
        "layout (triangle_strip, max_vertices = 4) out;\n"
        "\n"
        "in PARTICLE {\n"
        "    vec4 position;\n"
        "    vec2 uv;\n"
        "} gs_in[];\n"
        "\n"
        "out PARTICLE {\n"
        "    vec4 position;\n"
        "    vec2 uv;\n"
        "} gs_out;\n"
        "\n"
        "void main() {\n"
        "    gs_out.uv = vec2(0, 0);\n"
        "    gs_out.position.xy = gs_in[0].position.xy + gs_out.uv - vec2(0.5);\n"
        "    gl_Position = gs_out.position;\n"
        "    EmitVertex();\n"
        "    gs_out.uv = vec2(1, 0);\n"
        "    gs_out.position.xy = gs_in[0].position.xy + gs_out.uv - vec2(0.5);\n"
        "    gl_Position = gs_out.position;\n"
        "    EmitVertex();\n"
        "    gs_out.uv = vec2(0, 1);\n"
        "    gs_out.position.xy = gs_in[0].position.xy + gs_out.uv - vec2(0.5);\n"
        "    gl_Position = gs_out.position;\n"
        "    EmitVertex();\n"
        "    gs_out.uv = vec2(1, 1);\n"
        "    gs_out.position.xy = gs_in[0].position.xy + gs_out.uv - vec2(0.5);\n"
        "    gl_Position = gs_out.position;\n"
        "    EmitVertex();\n"
        "    EndPrimitive();\n"
        "}\n"

        "\n";
        return std::make_shared<Shader>(src, GL_GEOMETRY_SHADER);
    }

    std::shared_ptr<Shader> Shader::standardFragmentDotParticleSystem() {
        std::string src =
        "#version 400 core\n\n"

        "in PARTICLE {\n"
        "    vec4 position;\n"
        "    vec2 uv;\n"
        "} fs_in;\n"
        "\n"
        "uniform vec3 particle_color;\n"
        "\n"
        "out vec4 FragColor;\n"

        "void main() {\n"
        "    FragColor = vec4(particle_color, 1);\n"
        "}\n"

        "\n";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }

    std::shared_ptr<Shader> Shader::standardFragmentTexturedParticleSystem() {
        std::string src =
        "#version 400 core\n\n"

        "in PARTICLE {\n"
        "    vec4 position;\n"
        "    vec2 uv;\n"
        "} fs_in;\n"
        "\n"
        "uniform vec3 particle_color;\n"
        "uniform sampler2D particle_texture;\n"
        "\n"
        "out vec4 FragColor;\n"

        "void main() {\n"
        "    FragColor = vec4(texture(particle_texture, fs_in.uv).xyz * particle_color, 1.0);\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }

}
