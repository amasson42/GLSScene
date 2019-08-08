
#include "BigChunk.hpp"

GameVoxelChunk::GameVoxelChunk() {
    node = std::make_shared<GLS::Node>();
    voxel = std::make_shared<GLS::VoxelChunk>();
    mesh = nullptr;
    mustUpdateMesh = true;
}

void GameVoxelChunk::updateMesh() {
    node->renderables().clear();
    mesh = GLS::Mesh::voxelChunk(voxel, false);
    node->addRenderable(mesh);
    mustUpdateMesh = false;
}

void GameVoxelChunk::setBlockAt(glm::ivec3 coord, int blockId) {
	voxel->setBlockIdAt(coord.x, coord.y, coord.z, blockId);
	voxel->calculBlockAdjacence(); // TODO: update only the block
	mustUpdateMesh = true;

    // TODO: save adjacents GameVoxelChunks in the struct and update them here
    std::array<std::shared_ptr<GLS::VoxelChunk>, 6> adjacents = voxel->adjacentChunks();
    if (adjacents[0] != nullptr && coord.x == CHUNKSIZE - 1) {

    }
}
