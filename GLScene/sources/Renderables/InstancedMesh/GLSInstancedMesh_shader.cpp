//
//  GLSInstancedMesh_shader.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {
    
    std::string InstancedMesh::shaderUniformsVertex() {
        return
        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec3 normal;\n"
        "layout (location = 2) in vec3 tangent;\n"
        "layout (location = 3) in vec3 bitangent;\n"
        "layout (location = 4) in vec2 uv;\n"
        "layout (location = 5) in mat4 model_matrix;\n"
        "\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
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

    std::string InstancedMesh::shaderUniformsGeometry() {
        return
        "in VS_OUT {\n"
        "    vec4 position;\n"
        "    vec3 wposition;\n"
        "    vec2 uv;\n"
        "    vec3 wnormal;\n"
        "    vec3 wtangent;\n"
        "    vec3 wbitangent;\n"
        "} gs_in;\n"
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

    std::string InstancedMesh::shaderUniformsFragment() {
        return Mesh::shaderUniformsFragment();
    }

}
