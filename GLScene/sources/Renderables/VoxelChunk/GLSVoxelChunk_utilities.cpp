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

	static bool _blockMustShowEdge(VoxelBlock block, VoxelBlock neighbor, VoxelChunkEdge edge, std::function<bool(VoxelBlock, VoxelBlock, VoxelChunkEdge)> delegate) {
		if (block.meshType == VoxelBlockMeshType::Full
			&& neighbor.meshType == VoxelBlockMeshType::Full) {
			if (delegate)
				return delegate(block, neighbor, edge);
			return false;
		}
		return true;
	}

    void VoxelChunk::_setBlockAdjacence(glm::ivec3 coord) {
		
		VoxelBlock& block(blockAt(coord));
		block._adjacent = 0;
		if (block.meshType != VoxelBlockMeshType::Full) { // TODO: clean that
			block._adjacent = 0b00111111;
			return;
		}
        
		auto checkEdge = [this, coord, &block](VoxelChunkEdge edge, bool onEdge, glm::ivec3 offset, glm::ivec3 neighborPlace) {
			if (onEdge) {
				if (_adjChunks[edge].expired()
					|| _blockMustShowEdge(block, _adjChunks[edge].lock()->blockAt(neighborPlace), edge, _adjacentFunction))
					block._adjacent |= (1 << (edge));
			} else if (_blockMustShowEdge(block, blockAt(coord + offset), edge, _adjacentFunction))
				block._adjacent |= (1 << (edge));
		};

		checkEdge(VoxelChunkEdge::Positive_X, coord.x == VoxelChunk::chunkSize - 1, glm::ivec3(1, 0, 0), glm::ivec3(0, coord.y, coord.z));
		checkEdge(VoxelChunkEdge::Negative_X, coord.x == 0, glm::ivec3(-1, 0, 0), glm::ivec3(VoxelChunk::chunkSize - 1, coord.y, coord.z));

		checkEdge(VoxelChunkEdge::Positive_Y, coord.y == VoxelChunk::chunkSize - 1, glm::ivec3(0, 1, 0), glm::ivec3(coord.x, 0, coord.z));
		checkEdge(VoxelChunkEdge::Negative_Y, coord.y == 0, glm::ivec3(0, -1, 0), glm::ivec3(coord.x, VoxelChunk::chunkSize - 1, coord.z));

		checkEdge(VoxelChunkEdge::Positive_Z, coord.z == VoxelChunk::chunkSize - 1, glm::ivec3(0, 0, 1), glm::ivec3(coord.x, coord.y, 0));
		checkEdge(VoxelChunkEdge::Negative_Z, coord.z == 0, glm::ivec3(0, 0, -1), glm::ivec3(coord.x, coord.y, VoxelChunk::chunkSize - 1));
        
    }

    void VoxelChunk::calculBlockAdjacence() {
		glm::ivec3 coord;
        for (coord.x = 0; coord.x < chunkSize; coord.x++)
            for (coord.y = 0; coord.y < chunkSize; coord.y++)
                for (coord.z = 0; coord.z < chunkSize; coord.z++) {
                    _setBlockAdjacence(coord);
                }
    }

    void VoxelChunk::calculBlockAdjacence(glm::ivec3 coord) {
        std::array<std::shared_ptr<VoxelChunk>, 6> adjChunks = adjacentChunks();

        _setBlockAdjacence(coord);
		auto setAdjacentBlock = [this, adjChunks, coord](VoxelChunkEdge edge, bool onEdge, glm::ivec3 offset, glm::ivec3 neighborPlace) {
			if (onEdge) {
				if (adjChunks[edge] != nullptr)
					adjChunks[edge]->_setBlockAdjacence(neighborPlace);
			} else {
				_setBlockAdjacence(coord + offset);
			}
		};

		setAdjacentBlock(VoxelChunkEdge::Positive_X, coord.x == chunkSize - 1, glm::ivec3(1, 0, 0), glm::ivec3(0, coord.y, coord.z));
		setAdjacentBlock(VoxelChunkEdge::Negative_X, coord.x == 0, glm::ivec3(-1, 0, 0), glm::ivec3(VoxelChunk::chunkSize - 1, coord.y, coord.z));

		setAdjacentBlock(VoxelChunkEdge::Positive_Y, coord.y == chunkSize - 1, glm::ivec3(0, 1, 0), glm::ivec3(coord.x, 0, coord.z));
		setAdjacentBlock(VoxelChunkEdge::Negative_Y, coord.y == 0, glm::ivec3(0, -1, 0), glm::ivec3(coord.x, VoxelChunk::chunkSize - 1, coord.z));

		setAdjacentBlock(VoxelChunkEdge::Positive_Z, coord.z == chunkSize - 1, glm::ivec3(0, 0, 1), glm::ivec3(coord.x, coord.y, 0));
		setAdjacentBlock(VoxelChunkEdge::Negative_Z, coord.z == 0, glm::ivec3(0, 0, -1), glm::ivec3(coord.x, coord.y, VoxelChunk::chunkSize - 1));

    }

	void VoxelChunk::calculBlockAdjacenceEdge(VoxelChunkEdge edge) {
        int xs, xe, ys, ye, zs, ze;
        const int cs = chunkSize - 1;
        switch (edge) {
            case Positive_X: xs = cs;        xe = chunkSize; ys = 0;         ye = chunkSize; zs = 0;         ze = chunkSize; break;
            case Negative_X: xs = 0;         xe = 1;         ys = 0;         ye = chunkSize; zs = 0;         ze = chunkSize; break;
            case Positive_Y: xs = 0;         xe = chunkSize; ys = cs;        ye = chunkSize; zs = 0;         ze = chunkSize; break;
            case Negative_Y: xs = 0;         xe = chunkSize; ys = 0;         ye = 1;         zs = 0;         ze = chunkSize; break;
            case Positive_Z: xs = 0;         xe = chunkSize; ys = 0;         ye = chunkSize; zs = cs;        ze = chunkSize; break;
            case Negative_Z: xs = 0;         xe = chunkSize; ys = 0;         ye = chunkSize; zs = 0;         ze = 1;         break;
            default: xs = xe = ys = ye = zs = ze = 0; break;
        }
        std::array<std::shared_ptr<VoxelChunk>, 6> adjChunks = adjacentChunks();
		glm::ivec3 coord;
        for (coord.x = xs; coord.x < xe; coord.x++)
            for (coord.y = ys; coord.y < ye; coord.y++)
                for (coord.z = zs; coord.z < ze; coord.z++)
                    _setBlockAdjacence(coord);
	}

    void VoxelChunk::setMaterial(std::shared_ptr<Material> mat) {
        _material = mat;
    }

    std::shared_ptr<Material> VoxelChunk::getMaterial() const {
        return _material;
    }

}
