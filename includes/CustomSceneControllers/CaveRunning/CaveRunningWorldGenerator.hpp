
#pragma once

#include "GLScene.hpp"
#include "CaveMaze.hpp"

#define CHUNKSIZE (static_cast<int>(GLS::VoxelChunk::chunkSize))

int* initNoise(unsigned int seed);
double smoothNoise(double x, double y, double z);
double linearNoise(double x, double y, double z);

struct CaveRunningRoom {
    static const int voxelWidth = 2;
    static const int voxelHeight = 2;
    static const int voxelCount = voxelWidth * voxelWidth * voxelHeight;

    std::shared_ptr<GLS::Node> environementNode;
};

class CaveRunningWorldGenerator {

    /// The overall game seed;
    unsigned int _seed;

    /// The permanent device to use
    const std::shared_ptr<CLD::GPUDevice> _device;
    /// The command queue for async creation of voxels
    int _chunkCreationCommandQueueIndex;
    CLD::CommandQueue _chunkCreationCommandQueue;
    /// The buffer used to store the perlin noise permutations for different uses
    int _perlinPermutationBufferIndex;
    CLD::Buffer _perlinPermutationBuffer;

    /// The kernel that can create chunks in a cave world
    int _chunkCreationKernelIndex;
    CLD::Kernel _chunkCreationKernel;
    std::shared_ptr<GLS::Material> _worldMaterial;

    public:
    CaveRunningWorldGenerator(unsigned int seed,
        std::shared_ptr<CLD::GPUDevice> device = nullptr);
    virtual ~CaveRunningWorldGenerator();

    void initRoomGenerator(const std::string& filename);
    void deleteRoomGenerator();

    std::shared_ptr<CaveRunningRoom> generateRoom(glm::ivec2 roomPosition, CaveMaze::Room caveRoom);

};
