
#include "ProceduralWorldGenerator.hpp"

ProceduralWorldGenerator::ProceduralWorldGenerator() :
	_commandQueueIndex(-1),
	_kernelIndex(-1),
	_perlinPermutationBufferIndex(-1),
	_seed(0) {

	_device = GLS::getSharedDevice();

	if (_device == nullptr)
		throw std::runtime_error("GLS was not initialized");

	// With one queue, kernel will be executed one after each other
	_device->createCommandQueue(&_commandQueueIndex);
	if (_commandQueueIndex < 0)
		throw std::runtime_error("Could not initilize a new cl command queue");
}

ProceduralWorldGenerator::~ProceduralWorldGenerator() {
	if (_perlinPermutationBufferIndex >= 0)
		_device->destroyBuffer(_perlinPermutationBuffer);
	if (_kernelIndex >= 0)
		_device->destroyKernel(_kernelIndex);
	_device->destroyCommandQueue(_commandQueueIndex);
}

std::shared_ptr<BigChunk> ProceduralWorldGenerator::generateBigChunkAt(glm::ivec2 bigChunkPos) {

	if (_kernelIndex < 0)
		throw std::runtime_error("world generator was used before reading a kernel");
	if (_perlinPermutationBufferIndex < 0)
		throw std::runtime_error("world generator was used before setting a seed");
	std::shared_ptr<BigChunk> bc = std::make_shared<BigChunk>(usedMaterial);

	std::vector<int> blocks = std::vector<int>(BigChunk::bigChunkCount * GLS::VoxelChunk::chunkBlockCount);

	CLD::Kernel *k = _device->kernel(_kernelIndex);
	int blocksBufferIndex;

	CLD::Buffer blocksArrayPointersBuffer = _device->createFlagBuffer(BigChunk::bigChunkCount * sizeof(int) * GLS::VoxelChunk::chunkBlockCount, CL_MEM_USE_HOST_PTR, &blocks[0], &blocksBufferIndex);

	k->setArgument(0, _perlinPermutationBuffer);
	k->setArgument(1, blocksArrayPointersBuffer);
	k->setArgument(2, *(cl_int2*)&bigChunkPos);
	k->setArgument(3, CHUNKSIZE);
	k->setArgument(4, BigChunk::bigChunkWidth);
	
	_device->commandQueue(_commandQueueIndex)->runNDRangeKernel(*k, blocks.size());
	
	_device->commandQueue(_commandQueueIndex)->readBuffer(blocksArrayPointersBuffer, &blocks[0], blocks.size() * sizeof(int));
	_device->commandQueue(_commandQueueIndex)->finish();

	for (int arrayPointerIndex = 0; arrayPointerIndex < BigChunk::bigChunkCount; arrayPointerIndex++) {
		std::memcpy(bc->chunkAt(arrayPointerIndex).voxel->getBlocks(),
			&blocks[arrayPointerIndex * GLS::VoxelChunk::chunkBlockCount],
			sizeof(int) * GLS::VoxelChunk::chunkBlockCount);
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
