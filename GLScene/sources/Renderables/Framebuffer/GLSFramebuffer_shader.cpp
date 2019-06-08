//
//  GLSFramebuffer.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSFramebuffer.hpp"

namespace GLS {
    
    std::string Framebuffer::shaderUniformsVertex() {
        return
        "layout (location = 0) in vec2 position;\n"
        "\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "uniform vec3 u_camera_position;\n"
        "\n"
        "out VS_OUT {\n"
        "    vec2 uv;\n"
        "} vs_out;\n"
        "\n";
    }

    std::string Framebuffer::shaderUniformsGeometry() {
        return
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "uniform vec3 u_camera_position;\n"
        "\n"
        "in VS_OUT {\n"
        "    vec2 uv;\n"
        "} gs_in;\n"
        "\n"
        "out VS_OUT {\n"
        "    vec2 uv;\n"
        "} gs_out;\n"
        "\n";
    }

    std::string Framebuffer::shaderUniformsFragment() {
        return
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "uniform vec3 u_camera_position;\n"
        "\n"
        "in VS_OUT {\n"
        "    vec2 uv;\n"
        "} fs_in;\n"
        "\n"
        "uniform sampler2D screen_texture;\n"
        "\n"
        "out vec4 FragColor;\n"
        "\n";
    }

}
