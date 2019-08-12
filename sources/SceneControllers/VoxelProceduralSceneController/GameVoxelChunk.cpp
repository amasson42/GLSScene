
#include "BigChunk.hpp"

float GameVoxelChunk::meshmerizerIntensity = 0.0;

GameVoxelChunk::GameVoxelChunk() {
    node = std::make_shared<GLS::Node>();
    voxel = std::make_shared<GLS::VoxelChunk>();
    mesh = nullptr;
    mustUpdateMesh = true;
}

void GameVoxelChunk::updateMesh() {
    node->renderables().clear();
    mesh = GLS::Mesh::voxelChunk(voxel, [](glm::vec3& v) {
		glm::vec3 vmod = glm::vec3(
			fmod(v.x, GLS::VoxelChunk::chunkSize),
			fmod(v.y, GLS::VoxelChunk::chunkSize),
			fmod(v.z, GLS::VoxelChunk::chunkSize));
		v.x += linearNoise(vmod.x + 0.0834, vmod.y + 0.001342, vmod.z + 0.1931) * meshmerizerIntensity;
		v.y += linearNoise(vmod.x + 0.0834, vmod.y + 0.001342, vmod.z + 0.1931) * meshmerizerIntensity;
		v.z += linearNoise(vmod.x + 0.0834, vmod.y + 0.001342, vmod.z + 0.1931) * meshmerizerIntensity;
	}, false);
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
