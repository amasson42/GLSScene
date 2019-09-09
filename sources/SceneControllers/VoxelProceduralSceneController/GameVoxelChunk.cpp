
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
    mesh = voxel->bakeMesh();
	if (meshmerizerIntensity > 0.0f) {
		for (GLS::Vertex& ver : mesh->verticesRef()) {
			glm::vec3& v(ver.position);
			glm::vec3 vmod = glm::vec3(
				fmod(v.x, GLS::VoxelChunk::chunkSize),
				fmod(v.y, GLS::VoxelChunk::chunkSize),
				fmod(v.z, GLS::VoxelChunk::chunkSize));
			v.x += linearNoise(vmod.x + 0.0834, vmod.y + 0.001342, vmod.z + 0.1931) * meshmerizerIntensity;
			v.y += linearNoise(vmod.x + 0.0834, vmod.y + 0.001342, vmod.z + 0.1931) * meshmerizerIntensity;
			v.z += linearNoise(vmod.x + 0.0834, vmod.y + 0.001342, vmod.z + 0.1931) * meshmerizerIntensity;
		}
	}
    mesh->calculNormals();
    node->addRenderable(mesh);
    mesh->setCastShadowFace(GL_BACK);
    mustUpdateMesh = false;
}

void GameVoxelChunk::setBlockAt(glm::ivec3 coord, GLS::VoxelBlock block) {
	voxel->blockAt(coord) = block;
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

void GameVoxelChunk::setAdjacentChunk(std::shared_ptr<GameVoxelChunk> gameChunk, GLS::VoxelChunkEdge edge) {
    adjacents[edge] = gameChunk;
    voxel->setAdjacentChunk(gameChunk->voxel, edge);
}

void GameVoxelChunk::setAdjacentChunk(std::weak_ptr<GameVoxelChunk> gameChunk, GLS::VoxelChunkEdge edge) {
    adjacents[edge] = gameChunk;
    if (!gameChunk.expired())
        voxel->setAdjacentChunk(gameChunk.lock()->voxel, edge);
}
