#include "ProceduralWorldGenerator.hpp"

ProceduralWorldGenerator::ProceduralWorldGenerator() {
	// tgros WiP
	int* p = initNoise(time(NULL));


	_device.createContext();

	std::string sourceCode;
	std::string line;
	std::ifstream kernelCodeFile;

	kernelCodeFile.open ("assets/voxelProceduralGeneratorSources/hillsGenerator.cl");
	while(!kernelCodeFile.eof()) {
		getline(kernelCodeFile, line);
		sourceCode += line + '\n';
	}
	kernelCodeFile.close();

	const char *programs[1];
	programs[0] = sourceCode.c_str();

	try {
		_device.createProgram(programs, &_programIndex);
	} catch (const CLD::GPUDevice::BuildProgramException& e) {
		std::cout << e.what() << std::endl;
	}

	// With one queue, kernel will be executed one after each other
	_device.createCommandQueue(&_commandQueueIndex);
	_device.createKernel(_programIndex, "generateBigChunk", &_kernelIndex);
	_device.destroyProgram(_programIndex);

	
	_perlinPermutationBuffer = _device.createFlagBuffer(sizeof(int) * 512, CL_MEM_USE_HOST_PTR, p, &_perlinPermutationBufferIndex);
}

std::shared_ptr<BigChunk> ProceduralWorldGenerator::generateBigChunkAt(glm::ivec2 bigChunkPos) {

	std::shared_ptr<BigChunk> bc = std::make_shared<BigChunk>(usedMaterial);

	std::vector<int> blocks = std::vector<int>(BigChunk::bigChunkCount * GLS::VoxelChunk::chunkBlockCount);

	CLD::Kernel *k = _device.kernel(_kernelIndex);
	int blocksBufferIndex;

	CLD::Buffer blocksArrayPointersBuffer = _device.createFlagBuffer(BigChunk::bigChunkCount * sizeof(int) * GLS::VoxelChunk::chunkBlockCount, CL_MEM_USE_HOST_PTR, &blocks[0], &blocksBufferIndex);

	k->setArgument(0, _perlinPermutationBuffer);
	k->setArgument(1, blocksArrayPointersBuffer);
	k->setArgument(2, *(cl_int2*)&bigChunkPos);
	k->setArgument(3, CHUNKSIZE);
	k->setArgument(4, BigChunk::bigChunkWidth);
	
	int ret = _device.commandQueue(_commandQueueIndex)->runNDRangeKernel(*k, blocks.size());
	
	_device.commandQueue(_commandQueueIndex)->readBuffer(blocksArrayPointersBuffer, &blocks[0], blocks.size() * sizeof(int));
	_device.commandQueue(_commandQueueIndex)->finish();

	for (int arrayPointerIndex = 0; arrayPointerIndex < BigChunk::bigChunkCount; arrayPointerIndex++) {
		std::memcpy(bc->chunkAt(arrayPointerIndex).voxel->getBlocks(),
			&blocks[arrayPointerIndex * GLS::VoxelChunk::chunkBlockCount],
			sizeof(int) * GLS::VoxelChunk::chunkBlockCount);
	}

	_device.destroyBuffer(blocksBufferIndex);
	return bc;
}