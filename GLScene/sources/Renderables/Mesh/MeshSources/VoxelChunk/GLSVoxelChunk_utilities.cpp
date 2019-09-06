//
//  GLSVoxelChunk_utilities.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSVoxelChunk.hpp"

namespace GLS {

    uint32_t VoxelChunk::indexOfBlock(glm::ivec3 coord) {
        return (coord.z * chunkSize * chunkSize + coord.y * chunkSize + coord.x);
    }

    glm::ivec3 VoxelChunk::coordinatesOfBlock(uint32_t i) {
        return glm::ivec3(i % chunkSize,
                         (i % (chunkSize * chunkSize)) / chunkSize,
                         i / (chunkSize * chunkSize));
	}

    VoxelChunkEdge VoxelChunk::opposedEdge(VoxelChunkEdge f) {
        return static_cast<VoxelChunkEdge>(((f / 2) * 2) + (1 - (f % 2)));
    }

	const std::array<VoxelBlock, VoxelChunk::chunkBlockCount>& VoxelChunk::getBlocks() const {
		return _blocks;
	}

	std::array<VoxelBlock, VoxelChunk::chunkBlockCount>& VoxelChunk::getBlocks() {
		return _blocks;
	}

	const VoxelBlock& VoxelChunk::blockAt(uint32_t coord) const {
		return _blocks[coord];
	}

	VoxelBlock& VoxelChunk::blockAt(uint32_t coord) {
		return _blocks[coord];
	}

	const VoxelBlock& VoxelChunk::blockAt(glm::ivec3 coord) const {
		return _blocks[indexOfBlock(coord)];
	}

	VoxelBlock& VoxelChunk::blockAt(glm::ivec3 coord) {
		return _blocks[indexOfBlock(coord)];
	}

	VoxelBlock VoxelChunk::adjacentBlockOf(glm::ivec3 coord, VoxelChunkEdge edge) const {
		switch (edge) {
		case Positive_X:
			if (coord.x == VoxelChunk::chunkSize - 1) {
				return _adjChunks[edge].expired() ? VoxelBlock() : _adjChunks[edge].lock()->blockAt(glm::ivec3(0, coord.y, coord.z));
			} else {
				return blockAt(coord + glm::ivec3(1, 0, 0));
			}
			break;
		case Negative_X:
			if (coord.x == 0) {
				return _adjChunks[edge].expired() ? VoxelBlock() : _adjChunks[edge].lock()->blockAt(glm::ivec3(VoxelChunk::chunkSize - 1, coord.y, coord.z));
			} else {
				return blockAt(coord + glm::ivec3(-1, 0, 0));
			}
			break;
		case Positive_Y:
			if (coord.y == VoxelChunk::chunkSize - 1) {
				return _adjChunks[edge].expired() ? VoxelBlock() : _adjChunks[edge].lock()->blockAt(glm::ivec3(coord.x, 0, coord.z));
			} else {
				return blockAt(coord + glm::ivec3(0, 1, 0));
			}
			break;
		case Negative_Y:
			if (coord.y == 0) {
				return _adjChunks[edge].expired() ? VoxelBlock() : _adjChunks[edge].lock()->blockAt(glm::ivec3(coord.x, VoxelChunk::chunkSize - 1, coord.z));
			} else {
				return blockAt(coord + glm::ivec3(0, -1, 0));
			}
			break;
		case Positive_Z:
			if (coord.z == VoxelChunk::chunkSize - 1) {
				return _adjChunks[edge].expired() ? VoxelBlock() : _adjChunks[edge].lock()->blockAt(glm::ivec3(coord.x, coord.y, 0));
			} else {
				return blockAt(coord + glm::ivec3(0, 0, 1));
			}
			break;
		case Negative_Z:
			if (coord.z == 0) {
				return _adjChunks[edge].expired() ? VoxelBlock() : _adjChunks[edge].lock()->blockAt(glm::ivec3(coord.x, coord.y, VoxelChunk::chunkSize - 1));
			} else {
				return blockAt(coord + glm::ivec3(0, 0, -1));
			}
			break;
		}
	}

    void VoxelChunk::setAdjacentChunk(std::shared_ptr<VoxelChunk> adjChunk, VoxelChunkEdge edge) {
        _adjChunks[edge] = adjChunk;
    }

    void VoxelChunk::setAdjacentChunks(std::array<std::weak_ptr<VoxelChunk>, 6> adjChunks) {
        _adjChunks = adjChunks;
    }

    std::shared_ptr<VoxelChunk> VoxelChunk::adjacentChunk(VoxelChunkEdge edge) {
        return _adjChunks[edge].expired() ? nullptr : _adjChunks[edge].lock();
    }

    std::array<std::shared_ptr<VoxelChunk>, 6> VoxelChunk::adjacentChunks() {
        std::array<std::shared_ptr<VoxelChunk>, 6> adj;
        for (int i = 0; i < 6; i++)
            adj[i] = _adjChunks[i].expired() ? nullptr : _adjChunks[i].lock();
        return adj;
    }

	void VoxelChunk::setAdjacentFunction(std::function<bool(VoxelBlock, VoxelBlock, VoxelChunkEdge)> f) {
		_adjacentFunction = f;
	}

    void VoxelChunk::setMaterial(std::shared_ptr<Material> mat) {
        _material = mat;
    }

    std::shared_ptr<Material> VoxelChunk::getMaterial() const {
        return _material;
    }

}
