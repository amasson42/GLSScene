
#pragma once

#include "GLScene.hpp"

#include "CaveMaze.hpp"

class CaveRunningWorldGenerator;

/**
 * A cave running scene is the object managing the game system into a previously created scene.
 * As long as this object exist, the scene should be operational and ready to run.
 */
class CaveRunningGame {
    
    /// A usable CLD device
    std::shared_ptr<CLD::GPUDevice> _device;
    /// The scene of the controller
    GLS::Scene *_scene;

    std::shared_ptr<CaveRunningWorldGenerator> _generator;

    /// The current maze of the game
    std::shared_ptr<CaveMaze> _currentMaze;

    /// The node that contain the 3d map of the maze
    std::shared_ptr<GLS::Node> _mazeDisplayedNode;
    /// The node containing the current room
    std::shared_ptr<GLS::Node> _roomNode;

    public:

    CaveRunningGame(GLS::Scene& scene);
    virtual ~CaveRunningGame();

    /**
     * This function needs to be called at the start of the game.
     * It will instanciate the game with an initial state
     */
    void createNewGame();

    /**
     * Debug functionnality to display the map of the generated maze
     */
    void displayCaveScheme();

    /**
     * Create the room with the current player position
     * Generate the mesh, terrain nodes and entities
     */
    void createRoomNodes(glm::ivec2 position);
    
};

struct CaveRunningRoom {
    static const int voxelWidth = 4;
    static const int voxelHeight = 2;
    static const int voxelCount = voxelWidth * voxelHeight;

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
