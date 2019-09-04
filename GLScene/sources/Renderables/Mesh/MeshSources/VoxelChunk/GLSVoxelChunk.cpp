//
//  GLSVoxelChunk.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSVoxelChunk.hpp"

namespace GLS {

	VoxelBlock::VoxelBlock() :
	_adjacent(0),
	orientation(VoxelBlockOrientation::BlockOrientation_PositiveZ),
	meshType(VoxelBlockMeshType::Empty),
	textureId(0)
	{}

	VoxelBlock::VoxelBlock(VoxelBlockMeshType type, uint8_t id) :
	_adjacent(0),
	orientation(VoxelBlockOrientation::BlockOrientation_PositiveX),
	meshType(type),
	textureId(id)
	{}

	VoxelBlock::VoxelBlock(VoxelBlockOrientation orient, VoxelBlockMeshType type, uint8_t id) :
	_adjacent(0),
	orientation(orient),
	meshType(type),
	textureId(id)
	{}

	bool VoxelBlock::operator==(VoxelBlock const& other) {
		return meshType == other.meshType && orientation == other.orientation && textureId == other.textureId;
	}

	bool VoxelBlock::operator!=(VoxelBlock const& other) {
		return !(*this == other);
	}

    VoxelChunk::VoxelChunk() :
	_blocks(),
	_material(nullptr),
	_adjChunks(),
	_adjacentFunction()
    {}

    VoxelChunk::VoxelChunk(const VoxelChunk& copy) :
	_blocks(copy._blocks),
	_material(copy._material),
	_adjacentFunction()
    {}

    VoxelChunk::~VoxelChunk() {

    }

    VoxelChunk& VoxelChunk::operator=(const VoxelChunk& copy) {
		_blocks = copy._blocks;
		_material = copy._material;
		_adjChunks = std::array<std::weak_ptr<VoxelChunk>, 6>();
		return *this;
    }

}
