//
//  GLSVoxelChunk_shader.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSVoxelChunk.hpp"

namespace GLS {

    std::string VoxelChunk::shaderUniformsVertex() {
        return
        "layout (location = 0) in int blockId;\n"
        "\n"
        "const int chunkSize = " + std::to_string(chunkSize) + ";\n"
        "\n"
        "out GS_IN {\n"
        "    int position_x;\n"
        "    int position_y;\n"
        "    int position_z;\n"
        "    flat int blockAdj;\n"
        "    flat int blockId;\n"
        "} vs_out;\n"
        "\n";
    }

    std::string VoxelChunk::shaderUniformsGeometry() {
        return
        "uniform mat3 u_mat_normal;\n"
        "uniform mat4 u_mat_projection;\n"
        "uniform mat4 u_mat_view;\n"
        "uniform mat4 u_mat_model;\n"
        "\n"
        "in GS_IN {\n"
        "    int position_x;\n"
        "    int position_y;\n"
        "    int position_z;\n"
        "    flat int blockAdj;\n"
        "    flat int blockId;\n"
        "} gs_in[];\n"
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

    std::string VoxelChunk::shaderUniformsFragment() {
        return Mesh::shaderUniformsFragment();
    }

}
