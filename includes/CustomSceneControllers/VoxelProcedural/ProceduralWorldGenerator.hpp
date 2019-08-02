
#pragma once

#include "AppEnv.hpp"
#define CHUNKSIZE GLS::VoxelChunk::chunkSize

class BigChunk;

/**
 * the class that is able to generate chunks with cool environnements
 */
class ProceduralWorldGenerator {

	public:

	ProceduralWorldGenerator();
	std::shared_ptr<BigChunk> generateBigChunkAt(glm::ivec2 bigChunkPos);

	std::shared_ptr<GLS::Material> usedMaterial;

	private:

	// tgros WiP
	int _programIndex;
	int _commandQueueIndex;
	int _kernelIndex;
	int _perlinPermutationBufferIndex;
	CLD::Buffer _perlinPermutationBuffer;

	CLD::GPUDevice _device;
	// end WiP

};