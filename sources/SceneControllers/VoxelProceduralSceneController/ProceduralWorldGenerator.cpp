
#include "ProceduralWorldGenerator.hpp"

ProceduralWorldGenerator::ProceduralWorldGenerator() :
	_commandQueueIndices({ -1 }),
	_currentQueue(0),
	_kernelIndex(-1),
	_perlinPermutationBufferIndex(-1),
	_seed(0) {

	// _device = GLS::getSharedDevice();
	_device = std::make_shared<CLD::GPUDevice>();
	_device->createContext(false);

	if (_device == nullptr)
		throw std::runtime_error("GLS was not initialized");

	// With one queue, kernel will be executed one after each other
	for (int i = 0; i < _commandQueueIndices.size(); i++) {
		_device->createCommandQueue(&_commandQueueIndices[i]);
		if (_commandQueueIndices[i] < 0) {
			while (i-- > 0) {
				_device->destroyCommandQueue(_commandQueueIndices[i]);
			}
			throw std::runtime_error("Could not initilize a new cl command queue");
		}
	}
}

ProceduralWorldGenerator::~ProceduralWorldGenerator() {
	if (_perlinPermutationBufferIndex >= 0)
		_device->destroyBuffer(_perlinPermutationBuffer);
	if (_kernelIndex >= 0)
		_device->destroyKernel(_kernelIndex);
	for (auto commandQueueIndex : _commandQueueIndices) {
		_device->destroyCommandQueue(commandQueueIndex);
	}
}

bool transparancyAdjacence(GLS::VoxelBlock block, GLS::VoxelBlock neighbor, GLS::VoxelChunkEdge edge) {
	(void)edge;
	if (neighbor.textureId == BLOCK_LEAFS_MOUNTAIN)
		return true;
	if (neighbor.textureId == BLOCK_LEAFS_TREE)
		return true;
	if (block.textureId != BLOCK_WATER && neighbor.textureId == BLOCK_WATER)
		return true;
	return false;
}

static void _proceduralGenerateTreeAt(std::shared_ptr<BigChunk> bc, glm::ivec3 coord) {
	const float treeSize = 13 * (fabs(linearNoise(coord.x * 0.312 + 0.6237, coord.y * 0.931 + 0.912, coord.z * 0.125 + 0.2944)) + 0.8);
	glm::ivec3 offset = glm::ivec3(0);
	for (offset.y = 0; offset.y < 4; offset.y++) {
		bc->blockAt(coord + offset) = GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_WOOD);
	}
	for (offset.x = -2; offset.x <= 2; offset.x++) {
		for (offset.y = 2; offset.y <= 5; offset.y++) {
			for (offset.z = -2; offset.z <= 2; offset.z++) {
				if (bc->blockAt(coord + offset).meshType == GLS::VoxelBlockMeshType::Empty) {
					glm::ivec3 bo = offset - glm::ivec3(0, 1, 0);
					if (bo.x * bo.x + bo.y * bo.y + bo.z * bo.z < treeSize)
						bc->blockAt(coord + offset) = GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_LEAFS_TREE);
				}
			}
		}
	}
}

static void _proceduralGenerateTreeMountainAt(std::shared_ptr<BigChunk> bc, glm::ivec3 coord) {
	const float treeSize = 9 * (fabs(linearNoise(coord.x * 0.312 + 0.6237, coord.y * 0.931 + 0.912, coord.z * 0.125 + 0.2944)) + 1.2);
	glm::ivec3 offset = glm::ivec3(0);
	for (offset.y = 0; offset.y < 0.7 * treeSize; offset.y++) {
		bc->blockAt(coord + offset) = GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_WOOD);
	}
	for (offset.y = 3; offset.y <= treeSize; offset.y++) {
		const float width = std::max(-(offset.y - treeSize) * (cos(M_PI * offset.y) + 1) / 5, 2.5 - offset.y * 2.5 / treeSize);
		for (offset.x = -width; offset.x <= width; offset.x++) {
			for (offset.z = -width; offset.z <= width; offset.z++) {
				if (bc->blockAt(coord + offset).meshType == GLS::VoxelBlockMeshType::Empty) {
					bc->blockAt(coord + offset) = GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_LEAFS_MOUNTAIN);
				}
			}
		}
	}
}

static void _proceduralGenerateStructures(std::shared_ptr<BigChunk> bc) {
	glm::ivec3 coord;
	for (coord.x = 0; coord.x < BigChunk::bigChunkWidth * CHUNKSIZE; coord.x++) {
		for (coord.y = 0; coord.y < BigChunk::bigChunkHeight * CHUNKSIZE; coord.y++) {
			for (coord.z = 0; coord.z < BigChunk::bigChunkWidth * CHUNKSIZE; coord.z++) {
				if (bc->blockAt(coord) == GLS::VoxelBlock(GLS::VoxelBlockMeshType::Fence, BLOCK_WOOD))
					_proceduralGenerateTreeAt(bc, coord);
				if (bc->blockAt(coord) == GLS::VoxelBlock(GLS::VoxelBlockMeshType::Cross, BLOCK_WOOD))
					_proceduralGenerateTreeMountainAt(bc, coord);
			}
		}
	}
}

std::shared_ptr<BigChunk> ProceduralWorldGenerator::generateBigChunkAt(glm::ivec2 bigChunkPos) {
	if (_kernelIndex < 0)
		throw std::runtime_error("world generator was used before reading a kernel");
	if (_perlinPermutationBufferIndex < 0)
		throw std::runtime_error("world generator was used before setting a seed");
	std::shared_ptr<BigChunk> bc = std::make_shared<BigChunk>(usedMaterial);

	std::vector<GLS::VoxelBlock> blocks(GLS::VoxelChunk::chunkBlockCount * BigChunk::bigChunkCount);

	CLD::Kernel *k = _device->kernel(_kernelIndex);
	int blocksBufferIndex = 0;

	_generationMutex.lock();
	CLD::Buffer blocksArrayPointersBuffer = _device->createFlagBuffer(blocks.size() * sizeof(GLS::VoxelBlock), CL_MEM_USE_HOST_PTR, &(blocks.front()), &blocksBufferIndex);

	cl_int2 bcPos;
	bcPos.s[0] = bigChunkPos.x;
	bcPos.s[1] = bigChunkPos.y;

	k->setArgument(0, _perlinPermutationBuffer);
	k->setArgument(1, blocksArrayPointersBuffer);
	k->setArgument(2, bcPos);
	k->setArgument(3, CHUNKSIZE);
	k->setArgument(4, BigChunk::bigChunkWidth);
	
	_device->commandQueue(_commandQueueIndices[_currentQueue])->runNDRangeKernel(*k, blocks.size());
	
	_device->commandQueue(_commandQueueIndices[_currentQueue])->readBuffer(blocksArrayPointersBuffer, &(blocks.front()), blocks.size() * sizeof(GLS::VoxelBlock));
	_device->commandQueue(_commandQueueIndices[_currentQueue])->finish();
	_currentQueue = (_currentQueue + 1) % _commandQueueIndices.size();

	for (int arrayPointerIndex = 0; arrayPointerIndex < BigChunk::bigChunkCount; arrayPointerIndex++) {
		std::memcpy(&(bc->chunkAt(arrayPointerIndex)->voxel->getBlocks().front()),
			&(blocks[arrayPointerIndex * GLS::VoxelChunk::chunkBlockCount]),
			sizeof(GLS::VoxelBlock) * GLS::VoxelChunk::chunkBlockCount);
		bc->chunkAt(arrayPointerIndex)->voxel->setAdjacentFunction(transparancyAdjacence);
	}

	_device->destroyBuffer(blocksBufferIndex);

	_proceduralGenerateStructures(bc);

	_generationMutex.unlock();

	return bc;
}

void ProceduralWorldGenerator::setSeed(unsigned int seed) {
	if (_perlinPermutationBufferIndex >= 0)
		_device->destroyBuffer(_perlinPermutationBufferIndex);
	int* p = initNoise(seed);
	_perlinPermutationBuffer = _device->createFlagBuffer(sizeof(int) * 512, CL_MEM_USE_HOST_PTR, p, &_perlinPermutationBufferIndex);
	if (_perlinPermutationBufferIndex < 0) {
		_seed = 0;
		return;
	}
	_seed = seed;
}

unsigned int ProceduralWorldGenerator::getSeed() const {
	return _seed;
}

void ProceduralWorldGenerator::setGenerationKernel(std::string kernelName) {
	const std::string voidWorldSource = "kernel void generateBigChunk(__global int* ppm, __global int* blocks, const int2 bigChunkPos, const int chunkSize, const int bigChunkWidth) {}";
	std::string sourceCode;
	std::ifstream kernelCodeFile(kernelName.c_str());
	if (kernelCodeFile.is_open()) {
		std::string line;
		while(!kernelCodeFile.eof()) {
			getline(kernelCodeFile, line);
			sourceCode += line + '\n';
		}
		kernelCodeFile.close();
	} else {
		std::cerr << "could not open file " << kernelName << std::endl;
		sourceCode = voidWorldSource;
	}
	auto createKernel = [this](const std::string& sourceCode) {
		const char *programs[1];
		programs[0] = sourceCode.c_str();
		int programIndex;
		try {
			_device->createProgram(programs, &programIndex);
		} catch (const CLD::GPUDevice::BuildProgramException& e) {
			std::cout << e.what() << std::endl;
			return;
		}
		if (_kernelIndex >= 0)
			_device->destroyKernel(_kernelIndex);
		_device->createKernel(programIndex, "generateBigChunk", &_kernelIndex);
		_device->destroyProgram(programIndex);
	};
	createKernel(sourceCode);
	if (_kernelIndex < 0) {
		std::cerr << "no function generateBigChunk found in kernel " << kernelName << std::endl;
		createKernel(voidWorldSource);
	}
}
