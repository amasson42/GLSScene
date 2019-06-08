//
//  GLSSkybox_shader.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSSkybox.hpp"

namespace GLS {

    std::string Skybox::shaderUniformsVertex() {
        return
        "layout (location = 0) in vec3 position;\n"
        "\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "\n"
        "out VS_OUT {\n"
        "    vec3 uv;\n"
        "} vs_out;\n"
        "\n";
    }

    std::string Skybox::shaderUniformsGeometry() {
        return
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "\n"
        "in VS_OUT {\n"
        "    vec3 uv;\n"
        "} gs_in;\n"
        "\n"
        "out VS_OUT {\n"
        "    vec3 uv;\n"
        "} gs_out;\n"
        "\n";
    }

    std::string Skybox::shaderUniformsFragment() {
        return
        "in VS_OUT {\n"
        "    vec3 uv;\n"
        "} fs_in;\n"
        "\n"
        "uniform samplerCube u_skybox;\n"
        "\n"
        "out vec4 FragColor;\n"
        "\n";
    }

}
