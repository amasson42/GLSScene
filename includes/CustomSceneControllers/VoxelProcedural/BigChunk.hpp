
#pragma once

#include "AppEnv.hpp"
#define CHUNKSIZE GLS::VoxelChunk::chunkSize

/**
 * a GameVoxelChunk is the packing of a game node, a voxel object and its corresponding mesh in a struct
 * the voxel is only used for utilities purposes and always transformed into mesh for the render
 * when we modify the voxel values, we must set the mustUpdateMesh variable to true
 */
struct GameVoxelChunk {
	std::shared_ptr<GLS::Node> node;
	std::shared_ptr<GLS::VoxelChunk> voxel;
	std::shared_ptr<GLS::Mesh> mesh;
	bool mustUpdateMesh;
	std::array<std::weak_ptr<GameVoxelChunk>, 6> adjacents; // TODO: this

	GameVoxelChunk();
	void updateMesh();
	void setBlockAt(glm::ivec3 coord, int blockId);

	void setAdjacent(std::shared_ptr<GameVoxelChunk> chunk, int edge);
};

/**
 * A big chunk is an array of many game voxel chunks embeded into a big cube
 * it can use adjacency mechanism delegating to the voxel adjacency mechanism
 * it can be save and loaded from a file.
 */
class BigChunk {
	// TODO: load and save into files
	public:
	static const int bigChunkWidth = 4;
	static const int bigChunkHeight = 16;
	static const int bigChunkCount = bigChunkWidth * bigChunkWidth * bigChunkHeight;

	private:
	std::shared_ptr<GLS::Node> _node;
	std::array<GameVoxelChunk, bigChunkCount> _chunks;
	std::array<std::weak_ptr<BigChunk>, 4> _adjacents; // 0: +X | 1: -X | 2: +Z | 3: -Z

	public:

	BigChunk(std::shared_ptr<GLS::Material> material = nullptr);

	~BigChunk();

	void save(const std::string& fileName);
	void loadFromFile(const std::string& fileName);
	void loadFromStream(std::istream& stream);


	GameVoxelChunk& chunkAt(int i);
	GameVoxelChunk& chunkAt(int x, int y, int z);
	GameVoxelChunk* chunkAt(const glm::vec3& pos);

	/**
	 * update all the voxel adjacent chunks on the edge
	 */
	void setAdjacentBigChunk_positiveX(std::shared_ptr<BigChunk> adj);
	void setAdjacentBigChunk_negativeX(std::shared_ptr<BigChunk> adj);
	void setAdjacentBigChunk_positiveZ(std::shared_ptr<BigChunk> adj);
	void setAdjacentBigChunk_negativeZ(std::shared_ptr<BigChunk> adj);

	void calculAllAdjacences();
	void generateAllMeshes();

	/**
	 * return the root node of the big chunk
	 */
	std::shared_ptr<GLS::Node> getNode();

};