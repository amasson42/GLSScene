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
        "} vs_out;\n"
        "\n"

        "void main() {\n"
        "    vs_out.position = u_mat_projection * u_mat_view * u_mat_model * vec4(position, 1.0);\n"
        "    vs_out.velocity = velocity;\n"
        "    vs_out.color = color;\n"
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
        "    vec3 velocity;\n"
        "    vec3 color;\n"
        "} gs_in[];\n"
        "\n"
        "out PARTICLE {\n"
        "    vec4 position;\n"
        "    vec2 uv;\n"
        "    vec3 velocity;\n"
        "    vec3 color;\n"
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
        "    vec3 velocity;\n"
        "    vec3 color;\n"
        "} fs_in;\n"
        "\n"
        "uniform vec3 particle_color;\n"
        "\n"
        "out vec4 FragColor;\n"

        "void main() {\n"
        "    FragColor = vec4(fs_in.color + fs_in.velocity, 1);\n"
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
