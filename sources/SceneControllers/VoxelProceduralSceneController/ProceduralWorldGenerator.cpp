
#include "ProceduralWorldGenerator.hpp"

ProceduralWorldGenerator::ProceduralWorldGenerator() :
	_commandQueueIndices({ -1 }),
	_currentQueue(0),
	_kernelIndex(-1),
	_perlinPermutationBufferIndex(-1),
	_seed(0) {

	_device = GLS::getSharedDevice();

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
	if (block.textureId != 18 && neighbor.textureId == 18)
		return true;
	return false;
}

std::shared_ptr<BigChunk> ProceduralWorldGenerator::generateBigChunkAt(glm::ivec2 bigChunkPos) {
	if (_kernelIndex < 0)
		throw std::runtime_error("world generator was used before reading a kernel");
	if (_perlinPermutationBufferIndex < 0)
		throw std::runtime_error("world generator was used before setting a seed");
	std::shared_ptr<BigChunk> bc = std::make_shared<BigChunk>(usedMaterial);

	std::vector<GLS::VoxelBlock> blocks(GLS::VoxelChunk::chunkBlockCount * BigChunk::bigChunkCount);

	CLD::Kernel *k = _device->kernel(_kernelIndex);
	int blocksBufferIndex;

	CLD::Buffer blocksArrayPointersBuffer = _device->createFlagBuffer(blocks.size() * sizeof(GLS::VoxelBlock), CL_MEM_USE_HOST_PTR, &(blocks.front()), &blocksBufferIndex);

	cl_int2 bcPos;
	bcPos.s[0] = bigChunkPos.x;
	bcPos.s[1] = bigChunkPos.y;

	_generationMutex.lock();
	k->setArgument(0, _perlinPermutationBuffer);
	k->setArgument(1, blocksArrayPointersBuffer);
	k->setArgument(2, bcPos);
	k->setArgument(3, CHUNKSIZE);
	k->setArgument(4, BigChunk::bigChunkWidth);
	
	_device->commandQueue(_commandQueueIndices[_currentQueue])->runNDRangeKernel(*k, blocks.size());
	_generationMutex.unlock();
	
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
	std::string sourceCode;
	std::string line;
	std::ifstream kernelCodeFile;
	kernelCodeFile.open(kernelName.c_str());
	if (!kernelCodeFile.is_open())
		throw std::runtime_error(("could not open file " + kernelName).c_str());
	while(!kernelCodeFile.eof()) {
		getline(kernelCodeFile, line);
		sourceCode += line + '\n';
	}
	kernelCodeFile.close();
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
}
