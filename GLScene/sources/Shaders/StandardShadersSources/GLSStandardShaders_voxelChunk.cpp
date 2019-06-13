//
//  GLSStandardShaders_voxelChunk.cpp
//  GLScene
//
//  Created by Arthur MASSON on 4/27/18.
//  Copyright © 2018 Arthur MASSON. All rights reserved.
//

#include "GLSShader.hpp"

namespace GLS {

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

        "vec3 origin;\n"
        "vec3 axe_x;\n"
        "vec3 axe_y;\n"
        "vec3 axe_z;\n"

        "vec2 getBlockIdUvs(vec2 uv, int blockId, int face) {\n" // face: 0 = bot; 1 = side; 2 = top
        "    blockId--;\n"
        "    int gridX = (blockId % 10);\n"
        "    int gridY = (blockId / 10);\n"
        "    uv.x = ((gridX * 3) + face + uv.x) * (1.0 / 30.0);\n"
        "    uv.y = (gridY + (1 - uv.y)) * (1.0 / 10.0);\n"
        "    return uv;\n"
        "}\n"

        // +X
        "void drawFace_positiveX_emitVertex(vec2 uv) {\n"
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

        "void drawFace_positiveX() {\n"
        "    drawFace_positiveX_emitVertex(vec2(uvs.x, uvs.x));\n"
        "    drawFace_positiveX_emitVertex(vec2(uvs.y, uvs.x));\n"
        "    drawFace_positiveX_emitVertex(vec2(uvs.x, uvs.y));\n"
        "    drawFace_positiveX_emitVertex(vec2(uvs.y, uvs.y));\n"
        "    EndPrimitive();\n"
        "}\n"

        // -X
        "void drawFace_negativeX_emitVertex(vec2 uv) {\n"
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
    
        "void drawFace_negativeX() {\n"
        "    drawFace_negativeX_emitVertex(vec2(uvs.x, uvs.x));\n"
        "    drawFace_negativeX_emitVertex(vec2(uvs.y, uvs.x));\n"
        "    drawFace_negativeX_emitVertex(vec2(uvs.x, uvs.y));\n"
        "    drawFace_negativeX_emitVertex(vec2(uvs.y, uvs.y));\n"
        "    EndPrimitive();\n"
        "}\n"

        // +Y
        "void drawFace_positiveY_emitVertex(vec2 uv) {\n"
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

        "void drawFace_positiveY() {\n"
        "    drawFace_positiveY_emitVertex(vec2(uvs.x, uvs.x));\n"
        "    drawFace_positiveY_emitVertex(vec2(uvs.y, uvs.x));\n"
        "    drawFace_positiveY_emitVertex(vec2(uvs.x, uvs.y));\n"
        "    drawFace_positiveY_emitVertex(vec2(uvs.y, uvs.y));\n"
        "    EndPrimitive();\n"
        "}\n"

        // -Y
        "void drawFace_negativeY_emitVertex(vec2 uv) {\n"
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

        "void drawFace_negativeY() {\n"
        "    drawFace_negativeY_emitVertex(vec2(uvs.x, uvs.x));\n"
        "    drawFace_negativeY_emitVertex(vec2(uvs.y, uvs.x));\n"
        "    drawFace_negativeY_emitVertex(vec2(uvs.x, uvs.y));\n"
        "    drawFace_negativeY_emitVertex(vec2(uvs.y, uvs.y));\n"
        "    EndPrimitive();\n"
        "}\n"

        // +Z
        "void drawFace_positiveZ_emitVertex(vec2 uv) {\n"
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

        "void drawFace_positiveZ() {\n"
        "    drawFace_positiveZ_emitVertex(vec2(uvs.x, uvs.x));\n"
        "    drawFace_positiveZ_emitVertex(vec2(uvs.y, uvs.x));\n"
        "    drawFace_positiveZ_emitVertex(vec2(uvs.x, uvs.y));\n"
        "    drawFace_positiveZ_emitVertex(vec2(uvs.y, uvs.y));\n"
        "    EndPrimitive();\n"
        "}\n"

        // -Z
        "void drawFace_negativeZ_emitVertex(vec2 uv) {\n"
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

        "void drawFace_negativeZ() {\n"
        "    drawFace_negativeZ_emitVertex(vec2(uvs.x, uvs.x));\n"
        "    drawFace_negativeZ_emitVertex(vec2(uvs.y, uvs.x));\n"
        "    drawFace_negativeZ_emitVertex(vec2(uvs.x, uvs.y));\n"
        "    drawFace_negativeZ_emitVertex(vec2(uvs.y, uvs.y));\n"
        "    EndPrimitive();\n"
        "}\n"

        "void main() {\n"
        "    axe_x = vec3(u_mat_model * vec4(1, 0, 0, 0));\n"
        "    axe_y = vec3(u_mat_model * vec4(0, 1, 0, 0));\n"
        "    axe_z = vec3(u_mat_model * vec4(0, 0, 1, 0));\n"
        "    origin = vec3(u_mat_model * vec4(0, 0, 0, 1))"
        "        + axe_x * gs_in[0].position_x"
        "        + axe_y * gs_in[0].position_y"
        "        + axe_z * gs_in[0].position_z;\n"

        "    if (gs_in[0].blockId == 0)\n"
        "        return;\n"

        "    if ((gs_in[0].blockAdj & (1 << 0)) != 0)\n"
        "        drawFace_positiveX();\n"
        "    if ((gs_in[0].blockAdj & (1 << 1)) != 0)\n"
        "        drawFace_negativeX();\n"
        "    if ((gs_in[0].blockAdj & (1 << 2)) != 0)\n"
        "        drawFace_positiveY();\n"
        "    if ((gs_in[0].blockAdj & (1 << 3)) != 0)\n"
        "        drawFace_negativeY();\n"
        "    if ((gs_in[0].blockAdj & (1 << 4)) != 0)\n"
        "        drawFace_positiveZ();\n"
        "    if ((gs_in[0].blockAdj & (1 << 5)) != 0)\n"
        "        drawFace_negativeZ();\n"

        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_GEOMETRY_SHADER);
    }

}
