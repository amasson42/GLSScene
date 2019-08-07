
#pragma once

#include "AppEnv.hpp"
#define CHUNKSIZE GLS::VoxelChunk::chunkSize

class ProceduralWorldGenerator;
class BigChunk;

/**
 * the class that will manage chunk loading/saving and all those shits
 */
class DynamicWorld {

public:

	DynamicWorld(std::shared_ptr<GLS::Node> worldNode, std::string worldName);
	glm::ivec2 worldToBigChunkPosition(glm::vec3 position);
	glm::vec3 bigChunkPositionToWorld(glm::ivec2 position);
	const std::string getBigChunkFileNameAt(glm::ivec2 position);

	/**
	 * the method called every frame
	 * send the position of the player
	 * will load the unloaded chunks near enough
	 * will unload the loaded chunks far enough
	 */
	void loadPosition(std::shared_ptr<GLS::Node> cameraNode);

	void setRenderDistance(float distance);
	float getRenderDistance() const;

	std::shared_ptr<ProceduralWorldGenerator> getGenerator();

	static const float minRenderDistance;
	static const float maxRenderDistance;

private:

	std::string _worldDirName;
	std::shared_ptr<GLS::Node> _worldNode;
	std::shared_ptr<ProceduralWorldGenerator> _generator;

	std::map<glm::ivec2, std::shared_ptr<BigChunk> > _loadedChunks;
	std::map<glm::ivec2, std::future<std::shared_ptr<BigChunk>>> _loadingChunks;
	float _loadingDistance;
	float _visibleDistance;

	void _cleanChunks(glm::vec3& cameraFlatPosition);
	void _generateChunks(glm::vec3& cameraFlatPosition, std::shared_ptr<GLS::Node> cameraNode);
	void _generateMeshes(std::shared_ptr<GLS::Node> cameraNode);

};