
#include "CustomSceneControllers/VoxelProcedural/BigChunk.hpp"

BigChunk::BigChunk(std::shared_ptr<GLS::Material> material) :
	_node(std::make_shared<GLS::Node>()),
	_chunks(),
	_adjacents(),
	_untouched(true) {

	for (int i = 0; i < bigChunkCount; i++)
		_chunks[i] = std::make_shared<GameVoxelChunk>();

	for (int x = 0; x < bigChunkWidth; x++) {
		for (int y = 0; y < bigChunkHeight; y++) {
			for (int z = 0; z < bigChunkWidth; z++) {
				std::weak_ptr<GameVoxelChunk> noChunkPtr;
				std::shared_ptr<GameVoxelChunk> chunk = chunkAt(x, y, z);
				chunk->voxel->setMaterial(material);

				chunk->setAdjacentChunk((x == bigChunkWidth - 1) ? noChunkPtr : chunkAt(x + 1, y, z), GLS::VoxelChunkEdge::Positive_X);
				chunk->setAdjacentChunk((x == 0) ? noChunkPtr : chunkAt(x - 1, y, z), GLS::VoxelChunkEdge::Negative_X);
				chunk->setAdjacentChunk((y == bigChunkHeight - 1) ? noChunkPtr : chunkAt(x, y + 1, z), GLS::VoxelChunkEdge::Positive_Y);
				chunk->setAdjacentChunk((y == 0) ? noChunkPtr : chunkAt(x, y - 1, z), GLS::VoxelChunkEdge::Negative_Y);
				chunk->setAdjacentChunk((z == bigChunkWidth - 1) ? noChunkPtr : chunkAt(x, y, z + 1), GLS::VoxelChunkEdge::Positive_Z);
				chunk->setAdjacentChunk((z == 0) ? noChunkPtr : chunkAt(x, y, z - 1), GLS::VoxelChunkEdge::Negative_Z);

				chunk->node->transform().setPosition(glm::vec3(CHUNKSIZE * x, CHUNKSIZE * y, CHUNKSIZE * z));
				chunk->node->setName("VX_" + std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z));
				chunk->mustUpdateMesh = true;
				_node->addChildNode(chunk->node);
			}
		}
	}
}

BigChunk::~BigChunk() {
	_node->removeFromParent();
}

void BigChunk::save(const std::string& fileName) {

	std::ofstream chunkFile(fileName.c_str(), std::ios::binary);
	
	if (!chunkFile.is_open()) {
		std::cerr << fileName << " error" << std::endl;
		return ;
	}
	for (int x = 0; x < bigChunkWidth; x++) {
		for (int y = 0; y < bigChunkHeight; y++) {
			for (int z = 0; z < bigChunkWidth; z++) {
				chunkFile.write((char*) chunkAt(x, y, z)->voxel->getBlocks().data(), sizeof(int) * GLS::VoxelChunk::chunkBlockCount);
			}
		}
	}
	chunkFile.close();

}

void BigChunk::loadFromFile(const std::string& fileName) {
	std::ifstream chunkFile(fileName.c_str(), std::ios::binary);
	
	if (!chunkFile.is_open()) {
		std::cerr << fileName << " error" << std::endl;
		return ;
	}

	loadFromStream(chunkFile);

	chunkFile.close();
}

void BigChunk::loadFromStream(std::istream& stream) {
	for (int x = 0; x < bigChunkWidth; x++) {
		for (int y = 0; y < bigChunkHeight; y++) {
			for (int z = 0; z < bigChunkWidth; z++) {
				stream.read((char*) chunkAt(x, y, z)->voxel->getBlocks().data(), sizeof(int) * GLS::VoxelChunk::chunkBlockCount);
			}
		}
	}
}

bool BigChunk::isUntouched() const {
	return _untouched;
}

void BigChunk::setUntouched(bool untouched) {
	_untouched = untouched;
}

std::shared_ptr<GameVoxelChunk> BigChunk::chunkAt(int i) {
	if (i < 0 || i >= bigChunkCount)
		return nullptr;
	return _chunks[i];
}

std::shared_ptr<GameVoxelChunk> BigChunk::chunkAt(int x, int y, int z) {
	int index = bigChunkWidth * bigChunkWidth * y + bigChunkWidth * x + z;
	return chunkAt(index);
}

std::shared_ptr<GameVoxelChunk> BigChunk::chunkAt(const glm::vec3& pos) {
	int x = pos.x / CHUNKSIZE;
	int y = pos.y / CHUNKSIZE;
	int z = pos.z / CHUNKSIZE;
	return chunkAt(x, y, z);
}

GLS::VoxelBlock& BigChunk::blockAt(glm::ivec3 coord) {
	static GLS::VoxelBlock voidBlock;
	const glm::ivec3 bigChunkLimit = glm::ivec3(bigChunkWidth * CHUNKSIZE, bigChunkHeight * CHUNKSIZE, bigChunkWidth * CHUNKSIZE);
	if (coord.x < 0 || coord.y < 0 || coord.z < 0 || coord.x >= bigChunkLimit.x || coord.y >= bigChunkLimit.y || coord.z >= bigChunkLimit.z)
		return voidBlock;
	std::shared_ptr<GameVoxelChunk> chunk = chunkAt(coord.x / CHUNKSIZE, coord.y / CHUNKSIZE, coord.z / CHUNKSIZE);
	if (chunk == nullptr) {
		return voidBlock;
	}
	return chunk->voxel->blockAt(glm::ivec3(coord.x % 16, coord.y % 16, coord.z % 16));
}

void BigChunk::setAdjacentBigChunk_positiveX(std::shared_ptr<BigChunk> adj) {
	_adjacents[0] = adj;
	const int bcs = bigChunkWidth - 1;
	int x = bcs;
	for (int y = 0; y < bigChunkHeight; y++) {
		for (int z = 0; z < bigChunkWidth; z++) {
			std::shared_ptr<GameVoxelChunk> chunk = chunkAt(x, y, z);
			chunk->voxel->setAdjacentChunk(adj->chunkAt(0, y, z)->voxel, GLS::VoxelChunkEdge::Positive_X);
			chunk->mustUpdateMesh = true;
		}
	}
}

void BigChunk::setAdjacentBigChunk_negativeX(std::shared_ptr<BigChunk> adj) {
	_adjacents[1] = adj;
	const int bcs = bigChunkWidth - 1;
	int x = 0;
	for (int y = 0; y < bigChunkHeight; y++) {
		for (int z = 0; z < bigChunkWidth; z++) {
			std::shared_ptr<GameVoxelChunk> chunk = chunkAt(x, y, z);
			chunk->voxel->setAdjacentChunk(adj->chunkAt(bcs, y, z)->voxel, GLS::VoxelChunkEdge::Negative_X);
			chunk->mustUpdateMesh = true;
		}
	}
}

void BigChunk::setAdjacentBigChunk_positiveZ(std::shared_ptr<BigChunk> adj) {
	_adjacents[2] = adj;
	const int bcs = bigChunkWidth - 1;
	for (int x = 0; x < bigChunkWidth; x++) {
		for (int y = 0; y < bigChunkHeight; y++) {
			int z = bcs;
			std::shared_ptr<GameVoxelChunk> chunk = chunkAt(x, y, z);
			chunk->voxel->setAdjacentChunk(adj->chunkAt(x, y, 0)->voxel, GLS::VoxelChunkEdge::Positive_Z);
			chunk->mustUpdateMesh = true;
		}
	}
}

void BigChunk::setAdjacentBigChunk_negativeZ(std::shared_ptr<BigChunk> adj) {
	_adjacents[3] = adj;
	const int bcs = bigChunkWidth - 1;
	for (int x = 0; x < bigChunkWidth; x++) {
		for (int y = 0; y < bigChunkHeight; y++) {
			int z = 0;
			std::shared_ptr<GameVoxelChunk> chunk = chunkAt(x, y, z);
			chunk->voxel->setAdjacentChunk(adj->chunkAt(x, y, bcs)->voxel, GLS::VoxelChunkEdge::Negative_Z);
			chunk->mustUpdateMesh = true;
		}
	}
}

void BigChunk::generateAllMeshes() {
	for (int i = 0; i < BigChunk::bigChunkCount; i++) {
		chunkAt(i)->updateMesh();
	}
}

/**
 * return the root node of the big chunk
 */
std::shared_ptr<GLS::Node> BigChunk::getNode() {
	return _node;
}
