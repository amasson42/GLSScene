
#include "BigChunk.hpp"

float GameVoxelChunk::meshmerizerIntensity = 0.0;

GameVoxelChunk::GameVoxelChunk() {
    node = std::make_shared<GLS::Node>();
    voxel = std::make_shared<GLS::VoxelChunk>();
    mesh = nullptr;
    mustUpdateMesh = true;
}

void GameVoxelChunk::updateMesh() {
    voxel->calculBlockAdjacence();
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
    mesh->setCastShadowFace(GL_BACK);
    mustUpdateMesh = false;
}

void GameVoxelChunk::setBlockAt(glm::ivec3 coord, int blockId) {
	voxel->setBlockIdAt(coord.x, coord.y, coord.z, blockId);
	// voxel->calculBlockAdjacence(coord.x, coord.y, coord.z); // TODO: update only the block
	mustUpdateMesh = true;

    if (!adjacents[0].expired() && coord.x == CHUNKSIZE - 1)
        adjacents[0].lock()->mustUpdateMesh = true;
    if (!adjacents[1].expired() && coord.x == 0)
        adjacents[1].lock()->mustUpdateMesh = true;
    if (!adjacents[2].expired() && coord.y == CHUNKSIZE - 1)
        adjacents[2].lock()->mustUpdateMesh = true;
    if (!adjacents[3].expired() && coord.y == 0)
        adjacents[3].lock()->mustUpdateMesh = true;
    if (!adjacents[4].expired() && coord.z == CHUNKSIZE - 1)
        adjacents[4].lock()->mustUpdateMesh = true;
    if (!adjacents[5].expired() && coord.z == 0)
        adjacents[5].lock()->mustUpdateMesh = true;
}

void GameVoxelChunk::setAdjacentChunk(std::shared_ptr<GameVoxelChunk> gameChunk, int edge) {
    adjacents[edge] = gameChunk;
    voxel->setAdjacentChunk(gameChunk->voxel, edge);
}

void GameVoxelChunk::setAdjacentChunk(std::weak_ptr<GameVoxelChunk> gameChunk, int edge) {
    adjacents[edge] = gameChunk;
    if (!gameChunk.expired())
        voxel->setAdjacentChunk(gameChunk.lock()->voxel, edge);
}
