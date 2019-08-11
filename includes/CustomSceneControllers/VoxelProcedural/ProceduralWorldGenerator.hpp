
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
	virtual ~ProceduralWorldGenerator();
	std::shared_ptr<BigChunk> generateBigChunkAt(glm::ivec2 bigChunkPos);

	std::shared_ptr<GLS::Material> usedMaterial;

	void setSeed(unsigned int seed);
	unsigned int getSeed() const;

	void setGenerationKernel(std::string kernelName);

	private:

	int _commandQueueIndex;
	int _kernelIndex;
	int _perlinPermutationBufferIndex;
	unsigned int _seed;
	CLD::Buffer _perlinPermutationBuffer;

	std::shared_ptr<CLD::GPUDevice> _device;
	std::mutex _generationMutex;

};