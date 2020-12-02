//
//  GLSSkinnedMesh_shader.cpp
//  GLScene
//
//  Created by Arthur Masson on 26/06/2019.
//  Copyright © 2019 Arthur Masson. All rights reserved.
//

#include "GLSSkinnedMesh.hpp"
#include "GLSMesh.hpp"

namespace GLS {

    std::string SkinnedMesh::shaderUniformsVertex() {
        return
        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec3 normal;\n"
        "layout (location = 2) in vec3 tangent;\n"
        "layout (location = 3) in vec3 bitangent;\n"
        "layout (location = 4) in vec2 uv;\n"
        "layout (location = 5) in ivec4 joint_ids;\n"
        "layout (location = 6) in vec4 joint_weights;\n"
        "\n"
        "uniform mat3 u_mat_normal;\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "uniform vec3 u_camera_position;\n"
        "uniform mat4 u_mat_joints[" + std::to_string(Skeleton::maxBones) + "];\n"
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

    std::string SkinnedMesh::shaderUniformsGeometry() {
        return Mesh::shaderUniformsGeometry();
    }

    std::string SkinnedMesh::shaderUniformsFragment() {
        return Mesh::shaderUniformsFragment();
    }

}
