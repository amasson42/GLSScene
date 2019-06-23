//
//  GLSStandardShaders_particleSystem.cpp
//  GLScene
//
//  Created by Arthur MASSON on 4/27/18.
//  Copyright © 2018 Arthur MASSON. All rights reserved.
//

#include "GLSShader.hpp"

namespace GLS {

    std::shared_ptr<Shader> Shader::standardVertexParticleSystem() {
        std::string src =
        "#version 400 core\n\n"

        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec3 velocity;\n"
        "layout (location = 2) in vec3 color;\n"
        "layout (location = 3) in float size;\n"
        "layout (location = 4) in float age;\n"
        "\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "\n"
        "out PARTICLE {\n"
        "    vec4 position;\n"
        "    vec2 uv;\n"
        "    vec3 velocity;\n"
        "    vec3 color;\n"
        "    float size;\n"
        "    float age;\n"
        "} vs_out;\n"
        "\n"

        "void main() {\n"
        "    vs_out.position = u_mat_projection * u_mat_view * u_mat_model * vec4(position, 1.0);\n"
        "    vs_out.velocity = velocity;\n"
        "    vs_out.color = color;\n"
        "    vs_out.size = size;\n"
        "    vs_out.age = age;\n"
        "    gl_Position = vs_out.position;\n"
        "    gl_PointSize = vs_out.size;\n"
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
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "\n"
        "in PARTICLE {\n"
        "    vec4 position;\n"
        "    vec2 uv;\n"
        "    vec3 velocity;\n"
        "    vec3 color;\n"
        "    float size;\n"
        "    float age;\n"
        "} gs_in[];\n"
        "\n"
        "out PARTICLE {\n"
        "    vec4 position;\n"
        "    vec2 uv;\n"
        "    vec3 velocity;\n"
        "    vec3 color;\n"
        "    float size;\n"
        "    float age;\n"
        "} gs_out;\n"
        "\n"
        "void emitUv(vec2 uv) {\n"
        "    vec4 pos_offset = u_mat_projection * vec4(vec2(gs_in[0].size) * (uv - vec2(0.5)), 0, 1);\n"
        "    gs_out.position.xy = gs_in[0].position.xy + pos_offset.xy;\n"
        "    gs_out.position.zw = gs_in[0].position.zw;\n"
        "    gs_out.uv = uv;\n"
        "    gs_out.velocity = gs_in[0].velocity;\n"
        "    gs_out.color = gs_in[0].color;\n"
        "    gs_out.size = gs_in[0].size;\n"
        "    gs_out.age = gs_in[0].age;\n"
        "    gl_Position = gs_out.position;\n"
        "    EmitVertex();\n"
        "}\n"
        "\n"
        "void main() {\n"
        "    emitUv(vec2(0, 0));\n"
        "    emitUv(vec2(1, 0));\n"
        "    emitUv(vec2(0, 1));\n"
        "    emitUv(vec2(1, 1));\n"
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
        "    vec3 velocity;\n"
        "    vec3 color;\n"
        "    float size;\n"
        "    float age;\n"
        "} fs_in;\n"
        "\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main() {\n"
        "    FragColor = vec4(fs_in.color, 1);\n"
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
        "    vec3 velocity;\n"
        "    vec3 color;\n"
        "    float size;\n"
        "    float age;\n"
        "} fs_in;\n"
        "\n"
        "uniform sampler2D particle_texture;\n"
        "\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main() {\n"
        "    FragColor = texture(particle_texture, fs_in.uv);\n"
        "    if (FragColor.a == 0.0) discard;\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }

}
