//
//  GLSVoxelChunk_mesh.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSVoxelChunk.hpp"

namespace GLS {

    struct FaceDescriptor {
        std::pair<glm::vec2, glm::vec2> faceRange;
        glm::vec3 offset;
    };

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

    static void _drawFace_negativeX_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(0, 1, 0) * uv.y + glm::vec3(0, 0, 1) * uv.x;
        v.normal = glm::vec3(-1, 0, 0);
        v.tangent = glm::vec3(0, 0, 1);
        v.bitangent = glm::vec3(0, 1, 0);
        v.uv = _getBlockIdUvs(uv, blockId, VoxelChunkEdge::Negative_X);
        vertices.push_back(v);
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

    static void _drawFace_negativeY_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(1, 0, 0) * (1 - uv.x) + glm::vec3(0, 0, 1) * (1 - uv.y);
        v.normal = glm::vec3(0, -1, 0);
        v.tangent = glm::vec3(-1, 0, 0);
        v.bitangent = glm::vec3(0, 0, -1);
        v.uv = _getBlockIdUvs(uv, blockId, VoxelChunkEdge::Negative_Y);
        vertices.push_back(v);
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

    static void _drawFace_negativeZ_emitVertex(std::vector<Vertex>& vertices, glm::vec2 uv, int blockId, glm::vec3 coords) {
        Vertex v;
        v.position = coords + glm::vec3(1, 0, 0) * (1 - uv.x) + glm::vec3(0, 1, 0) * uv.y;
        v.normal = glm::vec3(0, 0, -1);
        v.tangent = glm::vec3(-1, 0, 0);
        v.bitangent = glm::vec3(0, 1, 0);
        v.uv = _getBlockIdUvs(uv, blockId, VoxelChunkEdge::Negative_Z);
        vertices.push_back(v);
    }

    static void _drawFace(std::vector<Vertex>& vertices, std::vector<GLuint>& indices,
                            int blockId, glm::vec3 coords,
                            std::function<void(std::vector<Vertex>&, glm::vec2, int, glm::vec3)> emitVertex,
                            FaceDescriptor face)
    {
        emitVertex(vertices, glm::vec2(face.faceRange.first.x, face.faceRange.first.y), blockId, coords + face.offset);
        emitVertex(vertices, glm::vec2(face.faceRange.second.x, face.faceRange.first.y), blockId, coords + face.offset);
        emitVertex(vertices, glm::vec2(face.faceRange.first.x, face.faceRange.second.y), blockId, coords + face.offset);
        emitVertex(vertices, glm::vec2(face.faceRange.second.x, face.faceRange.second.y), blockId, coords + face.offset);
        _indices_appendFace(indices, vertices.size());
    }

    static bool _adjacentFace(VoxelBlock block, VoxelBlock neighbor, VoxelChunkEdge edge) {
        switch (neighbor.meshType) {
        case Empty:
            return true;
        case Full:
            switch (block.meshType) {
                case Full: return false;
                case ReduceHeight: case SlabLow: return edge == Positive_Y;
                case SlabHigh: return edge == Negative_Y;
                default: return true;
            }
        case ReduceHeight:
            switch (block.meshType) {
                case Full: return edge != Positive_Y;
                case ReduceHeight: case SlabLow: return edge == Positive_Y || edge == Negative_Y;
                case SlabHigh: return edge != Positive_Y;
                default: return true;
            }
        case SlabLow:
            switch (block.meshType) {
                case Full: return edge != Positive_Y;
                case ReduceHeight: return true;
                case SlabLow: return edge == Positive_Y || edge == Negative_Y;
                case SlabHigh: return edge != Positive_Y;
                default: return true;
            }
        case SlabHigh:
            switch (block.meshType) {
                case Full: case ReduceHeight: case SlabLow: return edge != Negative_Y;
                case SlabHigh: return edge == Positive_Y || edge == Negative_Y;
                default: return true;
            }
        default:
            return true;
        }
    }

    std::shared_ptr<Mesh> VoxelChunk::bakeMesh() const {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

        std::vector<Vertex>& vertices(mesh->verticesRef());
        std::vector<GLuint>& indices(mesh->indicesRef());

        const std::array<std::function<void(std::vector<Vertex>&, glm::vec2, int, glm::vec3)>, 6> emitVertices = {{
            _drawFace_positiveX_emitVertex,
            _drawFace_negativeX_emitVertex,
            _drawFace_positiveY_emitVertex,
            _drawFace_negativeY_emitVertex,
            _drawFace_positiveZ_emitVertex,
            _drawFace_negativeZ_emitVertex
        }};

        using glm::vec2;
        using glm::vec3;
        const std::array<FaceDescriptor, 6> fullFaceDescriptors = {{
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 1)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 1)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 1)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 1)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 1)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 1)), glm::vec3(0)}
        }};
        const float reduceHeightValue = 0.8;
        const std::array<FaceDescriptor, 6> reduceHeightFaceDescriptors = {{
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, reduceHeightValue)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, reduceHeightValue)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 1)), glm::vec3(0, reduceHeightValue - 1, 0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 1)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, reduceHeightValue)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, reduceHeightValue)), glm::vec3(0)}
        }};
        const std::array<FaceDescriptor, 6> slabLowFaceDescriptors = {{
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 0.5)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 0.5)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 1)), glm::vec3(0, -0.5, 0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 1)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 0.5)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 0.5)), glm::vec3(0)}
        }};
        const std::array<FaceDescriptor, 6> slabHighFaceDescriptors = {{
            {std::make_pair<vec2, vec2>(vec2(0, 0.5), vec2(1, 1)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0.5), vec2(1, 1)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 1)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0), vec2(1, 1)), glm::vec3(0, 0.5, 0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0.5), vec2(1, 1)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0, 0.5), vec2(1, 1)), glm::vec3(0)}
        }};
        const float fenceWidth = 0.2;
        const float _fenceOffset = 0.5 - fenceWidth;
        const std::array<FaceDescriptor, 6> fenceFaceDescriptors = {{
            {std::make_pair<vec2, vec2>(vec2(0.5 - fenceWidth, 0), vec2(0.5 + fenceWidth, 1)), glm::vec3(-_fenceOffset, 0, 0)},
            {std::make_pair<vec2, vec2>(vec2(0.5 - fenceWidth, 0), vec2(0.5 + fenceWidth, 1)), glm::vec3(_fenceOffset, 0, 0)},
            {std::make_pair<vec2, vec2>(vec2(0.5 - fenceWidth, 0.5 - fenceWidth), vec2(0.5 + fenceWidth, 0.5 + fenceWidth)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0.5 - fenceWidth, 0.5 - fenceWidth), vec2(0.5 + fenceWidth, 0.5 + fenceWidth)), glm::vec3(0)},
            {std::make_pair<vec2, vec2>(vec2(0.5 - fenceWidth, 0), vec2(0.5 + fenceWidth, 1)), glm::vec3(0, 0, -_fenceOffset)},
            {std::make_pair<vec2, vec2>(vec2(0.5 - fenceWidth, 0), vec2(0.5 + fenceWidth, 1)), glm::vec3(0, 0, _fenceOffset)}
        }};

		glm::ivec3 coord;
        for (coord.x = 0; coord.x < VoxelChunk::chunkSize; coord.x++) {
            for (coord.y = 0; coord.y < VoxelChunk::chunkSize; coord.y++) {
                for (coord.z = 0; coord.z < VoxelChunk::chunkSize; coord.z++) {
					glm::vec3 coords = glm::vec3(coord);
					const VoxelBlock& block(blockAt(coord));

                    const auto drawCubeMesh = [this, coord, block, &vertices, &indices, coords, emitVertices](const std::array<FaceDescriptor, 6>& faceDescriptors) {
                        for (VoxelChunkEdge edge = Positive_X; edge <= Negative_Z; edge = VoxelChunkEdge(edge + 1)) {
                            VoxelBlock adj = adjacentBlockOf(coord, edge);
                            if (_adjacentFace(block, adj, edge) || (_adjacentFunction && _adjacentFunction(block, adj, edge)))
                                _drawFace(vertices, indices, block.textureId, coords, emitVertices[edge], faceDescriptors[edge]);
                        }
                    };

					switch (block.meshType) {
						case Empty:
							break;
						case Full:
                            drawCubeMesh(fullFaceDescriptors);
							break;
                        case ReduceHeight:
                            drawCubeMesh(reduceHeightFaceDescriptors);
                            break;
                        case SlabLow:
                            drawCubeMesh(slabLowFaceDescriptors);
                            break;
                        case SlabHigh:
                            drawCubeMesh(slabHighFaceDescriptors);
                            break;
                        case Stair:
                            break;
                        case StairReversed:
                            break;
                        case FullEdge:
                            break;
                        case Fence:
                            drawCubeMesh(fenceFaceDescriptors);
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
