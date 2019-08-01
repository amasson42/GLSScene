#include "BigChunk.hpp"

BigChunk::BigChunk(std::shared_ptr<GLS::Material> material) :
	_node(std::make_shared<GLS::Node>()),
	_chunks(),
	_adjacents() {

	for (int x = 0; x < bigChunkWidth; x++) {
		for (int y = 0; y < bigChunkHeight; y++) {
			for (int z = 0; z < bigChunkWidth; z++) {
				std::weak_ptr<GLS::VoxelChunk> noChunkPtr;
				GameVoxelChunk& chunk(chunkAt(x, y, z));
				chunk.voxel->setMaterial(material);
				chunk.voxel->setAdjacentChunks({
					x == bigChunkWidth - 1 ? noChunkPtr : chunkAt(x + 1, y, z).voxel,
					x == 0 ? noChunkPtr : chunkAt(x - 1, y, z).voxel,
					y == bigChunkHeight - 1 ? noChunkPtr : chunkAt(x, y + 1, z).voxel,
					y == 0 ? noChunkPtr : chunkAt(x, y - 1, z).voxel,
					z == bigChunkWidth - 1 ? noChunkPtr : chunkAt(x, y, z + 1).voxel,
					z == 0 ? noChunkPtr : chunkAt(x, y, z - 1).voxel
				});
				chunk.node->transform().setPosition(glm::vec3(CHUNKSIZE * x, CHUNKSIZE * y, CHUNKSIZE * z));
				chunk.node->setName("VX_" + std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z));
				chunk.mustUpdateMesh = true;
				_node->addChildNode(chunk.node);
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
				chunkFile.write((char*) chunkAt(x, y, z).voxel->getBlocks(), sizeof(int) * GLS::VoxelChunk::chunkBlockCount);
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

	for (int x = 0; x < bigChunkWidth; x++) {
		for (int y = 0; y < bigChunkHeight; y++) {
			for (int z = 0; z < bigChunkWidth; z++) {
				chunkFile.read((char*) chunkAt(x, y, z).voxel->getBlocks(), sizeof(int) * GLS::VoxelChunk::chunkBlockCount);
			}
		}
	}
	chunkFile.close();
}

void BigChunk::loadFromStream(std::istream& stream) {
	for (int x = 0; x < bigChunkWidth; x++) {
		for (int y = 0; y < bigChunkHeight; y++) {
			for (int z = 0; z < bigChunkWidth; z++) {
				stream.read((char*) chunkAt(x, y, z).voxel->getBlocks(), sizeof(int) * GLS::VoxelChunk::chunkBlockCount);
			}
		}
	}
}


GameVoxelChunk& BigChunk::chunkAt(int i) {
	if (i < 0 || i >= bigChunkCount)
		throw std::runtime_error("fuck off");
	return _chunks[i];
}

GameVoxelChunk& BigChunk::chunkAt(int x, int y, int z) {
	// TODO: remove when all is over
	if (x < 0 || x >= bigChunkWidth || y < 0 || y >= bigChunkHeight || z < 0 || z >= bigChunkWidth)
		throw std::runtime_error("bad values sent to bigChunk::chunkAt");
	int index = bigChunkWidth * bigChunkWidth * y + bigChunkWidth * x + z;
	return _chunks[index];
}

void BigChunk::setAdjacentBigChunk_positiveX(std::shared_ptr<BigChunk> adj) {
	_adjacents[0] = adj;
	const int bcs = bigChunkWidth - 1;
	int x = bcs;
	for (int y = 0; y < bigChunkHeight; y++) {
		for (int z = 0; z < bigChunkWidth; z++) {
			GameVoxelChunk& chunk(chunkAt(x, y, z));
			chunk.voxel->setAdjacentChunk(adj->chunkAt(0, y, z).voxel, 0);
			chunk.voxel->calculBlockAdjacenceEdge(0);
			chunk.mustUpdateMesh = true;
		}
	}
}

void BigChunk::setAdjacentBigChunk_negativeX(std::shared_ptr<BigChunk> adj) {
	_adjacents[1] = adj;
	const int bcs = bigChunkWidth - 1;
	int x = 0;
	for (int y = 0; y < bigChunkHeight; y++) {
		for (int z = 0; z < bigChunkWidth; z++) {
			GameVoxelChunk& chunk(chunkAt(x, y, z));
			chunk.voxel->setAdjacentChunk(adj->chunkAt(bcs, y, z).voxel, 1);
			chunk.voxel->calculBlockAdjacenceEdge(1);
			chunk.mustUpdateMesh = true;
		}
	}
}

void BigChunk::setAdjacentBigChunk_positiveZ(std::shared_ptr<BigChunk> adj) {
	_adjacents[2] = adj;
	const int bcs = bigChunkWidth - 1;
	for (int x = 0; x < bigChunkWidth; x++) {
		for (int y = 0; y < bigChunkHeight; y++) {
			int z = bcs;
			GameVoxelChunk& chunk(chunkAt(x, y, z));
			chunk.voxel->setAdjacentChunk(adj->chunkAt(x, y, 0).voxel, 4);
			chunk.voxel->calculBlockAdjacenceEdge(4);
			chunk.mustUpdateMesh = true;
		}
	}
}

void BigChunk::setAdjacentBigChunk_negativeZ(std::shared_ptr<BigChunk> adj) {
	_adjacents[3] = adj;
	const int bcs = bigChunkWidth - 1;
	for (int x = 0; x < bigChunkWidth; x++) {
		for (int y = 0; y < bigChunkHeight; y++) {
			int z = 0;
			GameVoxelChunk& chunk(chunkAt(x, y, z));
			chunk.voxel->setAdjacentChunk(adj->chunkAt(x, y, bcs).voxel, 5);
			chunk.voxel->calculBlockAdjacenceEdge(5);
			chunk.mustUpdateMesh = true;
		}
	}
}

void BigChunk::generateAllMeshes() {
	for (int x = 0; x < bigChunkWidth; x++) {
		for (int y = 0; y < bigChunkHeight; y++) {
			for (int z = 0; z < bigChunkWidth; z++) {
				GameVoxelChunk& chunk(chunkAt(x, y, z));
				chunk.voxel->calculBlockAdjacence();
				chunk.updateMesh();
			}
		}
	}
}

/**
 * return the root node of the big chunk
 */
std::shared_ptr<GLS::Node> BigChunk::getNode() {
	return _node;
}