//
//  GLSVoxelChunk_mesh.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSVoxelChunk.hpp"

namespace GLS {

    static glm::vec2 _getBlockIdUvs(glm::vec2 uv, int blockId, int face) {
        blockId--;
        int gridX = (blockId % 10);
        int gridY = (blockId / 10);
        uv.x = ((gridX * 3.0f) + face + uv.x) * (1.0f / 30.0f);
        uv.y = (gridY + (1.0f - uv.y)) * (1.0f / 10.0f);
        return uv;
    }

    static void _indices_appendFace(std::vector<GLuint>& indices, size_t verticesSize) {
        indices.push_back(static_cast<GLuint>(verticesSize - 4));
        indices.push_back(static_cast<GLuint>(verticesSize - 3));
        indices.push_back(static_cast<GLuint>(verticesSize - 2));
        indices.push_back(static_cast<GLuint>(verticesSize - 3));
        indices.push_back(static_cast<GLuint>(verticesSize - 1));
        indices.push_back(static_cast<GLuint>(verticesSize - 2));
    }

    static void _drawFace_positiveX_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(1, 0, 0) + glm::vec3(0, 1, 0) * uv.y + glm::vec3(0, 0, 1) * (1 - uv.x);
        v.normal = glm::vec3(1, 0, 0);
        v.tangent = glm::vec3(0, 0, -1);
        v.bitangent = glm::vec3(0, 1, 0);
        v.uv = _getBlockIdUvs(uv, blockId, 1);
        vertices.push_back(v);
    }

    static void _drawFace_positiveX(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int blockId, glm::vec3 coords) {
        _drawFace_positiveX_emitVertex(vertices, glm::vec2(0, 0), blockId, coords);
        _drawFace_positiveX_emitVertex(vertices, glm::vec2(1, 0), blockId, coords);
        _drawFace_positiveX_emitVertex(vertices, glm::vec2(0, 1), blockId, coords);
        _drawFace_positiveX_emitVertex(vertices, glm::vec2(1, 1), blockId, coords);
        _indices_appendFace(indices, vertices.size());
    }

    static void _drawFace_negativeX_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(0, 1, 0) * uv.y + glm::vec3(0, 0, 1) * uv.x;
        v.normal = glm::vec3(-1, 0, 0);
        v.tangent = glm::vec3(0, 0, 1);
        v.bitangent = glm::vec3(0, 1, 0);
        v.uv = _getBlockIdUvs(uv, blockId, 1);
        vertices.push_back(v);
    }

    static void _drawFace_negativeX(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int blockId, glm::vec3 coords) {
        _drawFace_negativeX_emitVertex(vertices, glm::vec2(0, 0), blockId, coords);
        _drawFace_negativeX_emitVertex(vertices, glm::vec2(1, 0), blockId, coords);
        _drawFace_negativeX_emitVertex(vertices, glm::vec2(0, 1), blockId, coords);
        _drawFace_negativeX_emitVertex(vertices, glm::vec2(1, 1), blockId, coords);
        _indices_appendFace(indices, vertices.size());
    }

    static void _drawFace_positiveY_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(1, 0, 0) * uv.x + glm::vec3(0, 1, 0) + glm::vec3(0, 0, 1) * (1 - uv.y);
        v.normal = glm::vec3(0, 1, 0);
        v.tangent = glm::vec3(1, 0, 0);
        v.bitangent = glm::vec3(0, 0, -1);
        v.uv = _getBlockIdUvs(uv, blockId, 2);
        vertices.push_back(v);
    }

    static void _drawFace_positiveY(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int blockId, glm::vec3 coords) {
        _drawFace_positiveY_emitVertex(vertices, glm::vec2(0, 0), blockId, coords);
        _drawFace_positiveY_emitVertex(vertices, glm::vec2(1, 0), blockId, coords);
        _drawFace_positiveY_emitVertex(vertices, glm::vec2(0, 1), blockId, coords);
        _drawFace_positiveY_emitVertex(vertices, glm::vec2(1, 1), blockId, coords);
        _indices_appendFace(indices, vertices.size());
    }

    static void _drawFace_negativeY_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(1, 0, 0) * (1 - uv.x) + glm::vec3(0, 0, 1) * (1 - uv.y);
        v.normal = glm::vec3(0, -1, 0);
        v.tangent = glm::vec3(-1, 0, 0);
        v.bitangent = glm::vec3(0, 0, -1);
        v.uv = _getBlockIdUvs(uv, blockId, 0);
        vertices.push_back(v);
    }

    static void _drawFace_negativeY(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int blockId, glm::vec3 coords) {
        _drawFace_negativeY_emitVertex(vertices, glm::vec2(0, 0), blockId, coords);
        _drawFace_negativeY_emitVertex(vertices, glm::vec2(1, 0), blockId, coords);
        _drawFace_negativeY_emitVertex(vertices, glm::vec2(0, 1), blockId, coords);
        _drawFace_negativeY_emitVertex(vertices, glm::vec2(1, 1), blockId, coords);
        _indices_appendFace(indices, vertices.size());
    }

    static void _drawFace_positiveZ_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(1, 0, 0) * uv.x + glm::vec3(0, 1, 0) * uv.y + glm::vec3(0, 0, 1);
        v.normal = glm::vec3(0, 0, 1);
        v.tangent = glm::vec3(1, 0, 0);
        v.bitangent = glm::vec3(0, 1, 0);
        v.uv = _getBlockIdUvs(uv, blockId, 1);
        vertices.push_back(v);
    }

    static void _drawFace_positiveZ(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int blockId, glm::vec3 coords) {
        _drawFace_positiveZ_emitVertex(vertices, glm::vec2(0, 0), blockId, coords);
        _drawFace_positiveZ_emitVertex(vertices, glm::vec2(1, 0), blockId, coords);
        _drawFace_positiveZ_emitVertex(vertices, glm::vec2(0, 1), blockId, coords);
        _drawFace_positiveZ_emitVertex(vertices, glm::vec2(1, 1), blockId, coords);
        _indices_appendFace(indices, vertices.size());
    }

    static void _drawFace_negativeZ_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(1, 0, 0) * (1 - uv.x) + glm::vec3(0, 1, 0) * uv.y;
        v.normal = glm::vec3(0, 0, -1);
        v.tangent = glm::vec3(-1, 0, 0);
        v.bitangent = glm::vec3(0, 1, 0);
        v.uv = _getBlockIdUvs(uv, blockId, 1);
        vertices.push_back(v);
    }

    static void _drawFace_negativeZ(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int blockId, glm::vec3 coords) {
        _drawFace_negativeZ_emitVertex(vertices, glm::vec2(0, 0), blockId, coords);
        _drawFace_negativeZ_emitVertex(vertices, glm::vec2(1, 0), blockId, coords);
        _drawFace_negativeZ_emitVertex(vertices, glm::vec2(0, 1), blockId, coords);
        _drawFace_negativeZ_emitVertex(vertices, glm::vec2(1, 1), blockId, coords);
        _indices_appendFace(indices, vertices.size());
    }
    

    std::shared_ptr<Mesh> Mesh::voxelChunk(std::shared_ptr<VoxelChunk> chunk, bool generateBuffers) {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

        std::vector<Vertex>& vertices(mesh->verticesRef());
        std::vector<GLuint>& indices(mesh->indicesRef());

        for (int x = 0; x < VoxelChunk::chunkSize; x++) {
            for (int y = 0; y < VoxelChunk::chunkSize; y++) {
                for (int z = 0; z < VoxelChunk::chunkSize; z++) {
                    int blockId = chunk->blockIdAt(x, y, z);
                    if (blockId == 0)
                        continue;
                    int blockAdj = chunk->blockAdjAt(x, y, z);
                    glm::vec3 coords(x, y, z);
                    if ((blockAdj & (1 << 0)) != 0)
                        _drawFace_positiveX(vertices, indices, blockId, coords);
                    if ((blockAdj & (1 << 1)) != 0)
                        _drawFace_negativeX(vertices, indices, blockId, coords);
                    if ((blockAdj & (1 << 2)) != 0)
                        _drawFace_positiveY(vertices, indices, blockId, coords);
                    if ((blockAdj & (1 << 3)) != 0)
                        _drawFace_negativeY(vertices, indices, blockId, coords);
                    if ((blockAdj & (1 << 4)) != 0)
                        _drawFace_positiveZ(vertices, indices, blockId, coords);
                    if ((blockAdj & (1 << 5)) != 0)
                        _drawFace_negativeZ(vertices, indices, blockId, coords);
                }
            }
        }
        mesh->setMaterial(chunk->getMaterial());
        if (generateBuffers)
            mesh->generateBuffers();
        return mesh;
    }

}
