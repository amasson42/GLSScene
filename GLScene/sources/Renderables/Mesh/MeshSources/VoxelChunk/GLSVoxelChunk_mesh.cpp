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
		const int textureLineCount = 16;
		const int faceXCount = 3;
		const int faceYCount = 2;
		const float inside = 2.0 / 18.0;
		uv = (uv - glm::vec2(0.5)) * glm::vec2(1.0 - inside) + glm::vec2(0.5);
        int gridX = (blockId % textureLineCount);
        int gridY = (blockId / textureLineCount);
        uv.x = ((gridX * faceXCount) + (face % faceXCount) + uv.x) / float(faceXCount * textureLineCount);
        uv.y = ((gridY * faceYCount) + (face / faceXCount) + (1 - uv.y)) / float(faceYCount * textureLineCount);
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
        v.uv = _getBlockIdUvs(uv, blockId, VoxelChunkEdge::Positive_X);
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
        v.uv = _getBlockIdUvs(uv, blockId, VoxelChunkEdge::Negative_X);
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
        v.uv = _getBlockIdUvs(uv, blockId, VoxelChunkEdge::Positive_Y);
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
        v.uv = _getBlockIdUvs(uv, blockId, VoxelChunkEdge::Negative_Y);
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
        v.uv = _getBlockIdUvs(uv, blockId, VoxelChunkEdge::Positive_Z);
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
        v.uv = _getBlockIdUvs(uv, blockId, VoxelChunkEdge::Negative_Z);
        vertices.push_back(v);
    }

    static void _drawFace_negativeZ(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int blockId, glm::vec3 coords) {
        _drawFace_negativeZ_emitVertex(vertices, glm::vec2(0, 0), blockId, coords);
        _drawFace_negativeZ_emitVertex(vertices, glm::vec2(1, 0), blockId, coords);
        _drawFace_negativeZ_emitVertex(vertices, glm::vec2(0, 1), blockId, coords);
        _drawFace_negativeZ_emitVertex(vertices, glm::vec2(1, 1), blockId, coords);
        _indices_appendFace(indices, vertices.size());
    }
    
    std::shared_ptr<Mesh> VoxelChunk::bakeMesh() const {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

        std::vector<Vertex>& vertices(mesh->verticesRef());
        std::vector<GLuint>& indices(mesh->indicesRef());

		glm::ivec3 coord;
        for (coord.x = 0; coord.x < VoxelChunk::chunkSize; coord.x++) {
            for (coord.y = 0; coord.y < VoxelChunk::chunkSize; coord.y++) {
                for (coord.z = 0; coord.z < VoxelChunk::chunkSize; coord.z++) {
					glm::vec3 coords = glm::vec3(coord);
					const VoxelBlock& block(blockAt(coord));
					switch (block.meshType) {
						case Empty:
							break;
						case Full:
							if ((block._adjacent & (1 << VoxelChunkEdge::Positive_X)) != 0)
								_drawFace_positiveX(vertices, indices, block.textureId, coords);
							if ((block._adjacent & (1 << VoxelChunkEdge::Negative_X)) != 0)
								_drawFace_negativeX(vertices, indices, block.textureId, coords);
							if ((block._adjacent & (1 << VoxelChunkEdge::Positive_Y)) != 0)
								_drawFace_positiveY(vertices, indices, block.textureId, coords);
							if ((block._adjacent & (1 << VoxelChunkEdge::Negative_Y)) != 0)
								_drawFace_negativeY(vertices, indices, block.textureId, coords);
							if ((block._adjacent & (1 << VoxelChunkEdge::Positive_Z)) != 0)
								_drawFace_positiveZ(vertices, indices, block.textureId, coords);
							if ((block._adjacent & (1 << VoxelChunkEdge::Negative_Z)) != 0)
								_drawFace_negativeZ(vertices, indices, block.textureId, coords);
							break;
						default:
							break;
					}
                }
            }
        }
        mesh->setMaterial(getMaterial());
        return mesh;
    }

}
