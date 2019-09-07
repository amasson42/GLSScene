
#pragma once

#include "AppEnv.hpp"

#define BLOCK_BEDROCK 0
#define BLOCK_STONE 1
#define BLOCK_DIRT 2
#define BLOCK_GRASS 3
#define BLOCK_SAND 4
#define BLOCK_GRAVEL 5
#define BLOCK_WATER 7
#define BLOCK_GRASS_BROWN 16
#define BLOCK_WOOD 17
#define BLOCK_GOLD 18
#define BLOCK_WOOD_PLANKS 19
#define BLOCK_BRICKS 20
#define BLOCK_COBBLESTONE 21
#define BLOCK_SANDSTONE 22
#define BLOCK_ICE 32
#define BLOCK_ICE_BROKEN 33
#define BLOCK_SNOW 34
#define BLOCK_OBSIDIAN 36
#define BLOCK_GRASS_PURPLE 48
#define BLOCK_CACTUS 49
#define BLOCK_BUSH 50
#define BLOCK_FLOWER 51
#define BLOCK_LEAFS_TREE 52
#define BLOCK_LEAFS_MOUNTAIN 53
#define BLOCK_BUSH_DEAD 54
#define BLOCK_MUSHROOM_RED 55
#define BLOCK_MUSHROOM_BROWN 56
#define BLOCK_TERRACOTTA_ORANGE 64
#define BLOCK_TERRACOTTA_BROWN 65
#define BLOCK_TERRACOTTA_YELLOW 66
#define BLOCK_CLAY 67
#define BLOCK_TNT 68

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

	static float meshmerizerIntensity;

	GameVoxelChunk();
	void updateMesh();
	void setBlockAt(glm::ivec3 coord, GLS::VoxelBlock block);

	void setAdjacentChunk(std::shared_ptr<GameVoxelChunk> chunk, GLS::VoxelChunkEdge edge);
	void setAdjacentChunk(std::weak_ptr<GameVoxelChunk> chunk, GLS::VoxelChunkEdge edge);
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
	std::array<std::shared_ptr<GameVoxelChunk>, bigChunkCount> _chunks;
	std::array<std::weak_ptr<BigChunk>, 4> _adjacents; // 0: +X | 1: -X | 2: +Z | 3: -Z
	bool _untouched;

	public:

	BigChunk(std::shared_ptr<GLS::Material> material = nullptr);

	~BigChunk();

	void save(const std::string& fileName);
	void loadFromFile(const std::string& fileName);
	void loadFromStream(std::istream& stream);
	bool isUntouched() const;
	void setUntouched(bool untouched);


	std::shared_ptr<GameVoxelChunk> chunkAt(int i);
	std::shared_ptr<GameVoxelChunk> chunkAt(int x, int y, int z);
	std::shared_ptr<GameVoxelChunk> chunkAt(const glm::vec3& pos);

	GLS::VoxelBlock& blockAt(glm::ivec3 coord);

	/**
	 * update all the voxel adjacent chunks on the edge
	 */
	void setAdjacentBigChunk_positiveX(std::shared_ptr<BigChunk> adj);
	void setAdjacentBigChunk_negativeX(std::shared_ptr<BigChunk> adj);
	void setAdjacentBigChunk_positiveZ(std::shared_ptr<BigChunk> adj);
	void setAdjacentBigChunk_negativeZ(std::shared_ptr<BigChunk> adj);

	void generateAllMeshes();

	/**
	 * return the root node of the big chunk
	 */
	std::shared_ptr<GLS::Node> getNode();

};