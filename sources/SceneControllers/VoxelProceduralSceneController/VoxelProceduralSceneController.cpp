
#include "AppEnv.hpp"

#define BLOCK_AIR 0
#define BLOCK_BEDROCK 1
#define BLOCK_STONE 2
#define BLOCK_DIRT 3
#define BLOCK_GRASS 4
#define BLOCK_SAND 5
#define BLOCK_GRAVEL 6
#define BLOCK_GRASS_BROWN 11
#define BLOCK_WOOD 12
#define BLOCK_LEAFS 13
#define BLOCK_WOOD_PLANKS 14
#define BLOCK_BRICKS 15
#define BLOCK_COBBLESTONE 16
#define BLOCK_ICE 21
#define BLOCK_ICE_BROKEN 22
#define BLOCK_OBSIDIAN 25
#define BLOCK_GRASS_PURPLE 31
#define BLOCK_GOLD 35
#define BLOCK_TNT 92

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

    GameVoxelChunk() {
        node = std::make_shared<GLS::Node>();
        voxel = std::make_shared<GLS::VoxelChunk>();
        mesh = nullptr;
        mustUpdateMesh = true;
    }

    void updateMesh() {
        node->renderables().clear();
        mesh = GLS::Mesh::voxelChunk(voxel);
        mesh->generateBuffers();
        node->addRenderable(mesh);
        mustUpdateMesh = false;
    }
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

    BigChunk(std::shared_ptr<GLS::Material> material = nullptr) :
    _node(std::make_shared<GLS::Node>()),
    _chunks(),
    _adjacents() {

        for (int x = 0; x < bigChunkWidth; x++) {
            for (int y = 0; y < bigChunkHeight; y++) {
                for (int z = 0; z < bigChunkWidth; z++) {
                    std::weak_ptr<GLS::VoxelChunk> noChunkPtr;
                    GameVoxelChunk& chunk(chunkAt(x, y, z));
                    chunk.voxel->setMaterial(material);
                    chunk.voxel->setAdjacentChunks({
                        x == bigChunkWidth - 1 ? noChunkPtr : chunkAt(x + 1, y, z).voxel,
                        x == 0 ? noChunkPtr : chunkAt(x - 1, y, z).voxel,
                        y == bigChunkHeight - 1 ? noChunkPtr : chunkAt(x, y + 1, z).voxel,
                        y == 0 ? noChunkPtr : chunkAt(x, y - 1, z).voxel,
                        z == bigChunkWidth - 1 ? noChunkPtr : chunkAt(x, y, z + 1).voxel,
                        z == 0 ? noChunkPtr : chunkAt(x, y, z - 1).voxel
                    });
                    chunk.node->transform().setPosition(glm::vec3(CHUNKSIZE * x, CHUNKSIZE * y, CHUNKSIZE * z));
                    _node->addChildNode(chunk.node);
                }
            }
        }
    }

    GameVoxelChunk& chunkAt(int x, int y, int z) {
        // TODO: remove when all is over
        if (x < 0 || x >= bigChunkWidth || y < 0 || y >= bigChunkHeight || z < 0 || z >= bigChunkWidth)
            throw std::runtime_error("bad values sent to bigChunk::chunkAt");
        int index = bigChunkWidth * bigChunkWidth * y + bigChunkWidth * x + z;
        return _chunks[index];
    }

    /**
     * update all the voxel adjacent chunks on the edge
     */
    // TODO: split this function into 4 function setAdjacent_positiveX, setAdjacent_negativeX, ...
    void setAdjacentBigChunk(std::shared_ptr<BigChunk> adj, int edge) {
        // TODO: remove when all is over
        if (edge < 0 || edge >= 4)
            throw std::runtime_error("bad value sent to bigChunk::setAdjacentBigChunk");
        _adjacents[edge] = adj;
        const int bcs = bigChunkWidth - 1;

        switch (edge) {
        case 0:
            for (int y = 0; y < bigChunkHeight; y++) {
                for (int z = 0; z < bigChunkWidth; z++) {
                    int x = bcs;
                    chunkAt(x, y, z).voxel->setAdjacentChunk(adj->chunkAt(0, y, z).voxel, 0);
                    chunkAt(x, y, z).voxel->calculBlockAdjacenceEdge(0);
                }
            }
            break;
        case 1:
            for (int y = 0; y < bigChunkHeight; y++) {
                for (int z = 0; z < bigChunkWidth; z++) {
                    int x = 0;
                    chunkAt(x, y, z).voxel->setAdjacentChunk(adj->chunkAt(bcs, y, z).voxel, 1);
                    chunkAt(x, y, z).voxel->calculBlockAdjacenceEdge(1);
                }
            }
            break;
        case 2:
            for (int x = 0; x < bigChunkWidth; x++) {
                for (int y = 0; y < bigChunkHeight; y++) {
                    int z = bcs;
                    chunkAt(x, y, z).voxel->setAdjacentChunk(adj->chunkAt(x, y, 0).voxel, 4);
                    chunkAt(x, y, z).voxel->calculBlockAdjacenceEdge(4);
                }
            }
            break;
        case 3:
            for (int x = 0; x < bigChunkWidth; x++) {
                for (int y = 0; y < bigChunkHeight; y++) {
                    int z = 0;
                    chunkAt(x, y, z).voxel->setAdjacentChunk(adj->chunkAt(x, y, bcs).voxel, 5);
                    chunkAt(x, y, z).voxel->calculBlockAdjacenceEdge(5);
                }
            }
            break;
        }
    }

    /**
     * iterate trought all of the voxels
     * - calcul all of their adjacent values
     * - create the meshes
     * - generate buffers 
     * (preferable to do it only once)
     */
    void generateAllMeshes() {
        for (int x = 0; x < bigChunkWidth; x++) {
            for (int y = 0; y < bigChunkHeight; y++) {
                for (int z = 0; z < bigChunkWidth; z++) {
                    GameVoxelChunk& chunk(chunkAt(x, y, z));
                    chunk.voxel->calculBlockAdjacence();
                    chunk.updateMesh();
                    chunk.mesh->generateBuffers();
                }
            }
        }
    }

    /**
     * return the root node of the big chunk
     */
    std::shared_ptr<GLS::Node> getNode() {
        return _node;
    }

};

/**
 * the class that is able to generate chunks with cool environnements
 */
class ProceduralWorldGenerator {


    public:

    std::shared_ptr<GLS::Material> usedMaterial;

    ProceduralWorldGenerator() {

    }

    std::shared_ptr<BigChunk> generateBigChunkAt(glm::vec3 wpos) {
        (void)wpos;
        std::shared_ptr<BigChunk> bc = std::make_shared<BigChunk>(usedMaterial);

        for (int cx = 0; cx < BigChunk::bigChunkWidth; cx++) {
            for (int cy = 0; cy < BigChunk::bigChunkHeight; cy++) {
                for (int cz = 0; cz < BigChunk::bigChunkWidth; cz++) {

                    for (int x = 0; x < CHUNKSIZE; x++) {
                        for (int y = 0; y < CHUNKSIZE; y++) {
                            for (int z = 0; z < CHUNKSIZE; z++) {
                                if (y < CHUNKSIZE / 2 || x == 5 || z == 5)
                                    bc->chunkAt(cx, cy, cz).voxel->setBlockIdAt(x, y, z, static_cast<int>(wpos.x));
                            }
                        }
                    }

                }
            }
        }
        return bc;
    }

};

/**
 * the class that will manage chunk loading/saving and all those shits
 */
class DynamicWorld {

    public:
    std::string _worldDirName;
    std::shared_ptr<GLS::Node> _worldNode;
    std::shared_ptr<ProceduralWorldGenerator> _generator;

    std::map<std::pair<int, int>, std::shared_ptr<BigChunk> > _loadedChunks;

    DynamicWorld(std::shared_ptr<GLS::Node> worldNode, std::string worldName) {
        _worldNode = worldNode;
        _worldDirName = worldName + "/";
        _generator = std::make_shared<ProceduralWorldGenerator>();
    }

    /**
     * the method called every frame
     * will load the unloaded chunks near enough
     * will unload the loaded chunks far enough
     */
    void loadPosition(glm::vec3 position) {
        (void)position;
    }

};

static std::shared_ptr<DynamicWorld> _dynamicWorld = nullptr;

VoxelProceduralSceneController::VoxelProceduralSceneController(std::shared_ptr<GLSWindow> window) :
ISceneController(window) {
    lt = 0;
}

VoxelProceduralSceneController::~VoxelProceduralSceneController() {

}

void VoxelProceduralSceneController::update() {
    ISceneController::update();
    if (!mustUpdate)
        return;
    if (_window.expired())
        return;
    float et = _window.lock()->elapsedTime();

    if (et - lt >= 0.3) {
        lt = et;
        _dynamicWorld->loadPosition(cameraNode->transform().position());
    }
}

void VoxelProceduralSceneController::makeScene() {

    GLS::Scene& scene(*_scene);
    if (_window.expired())
        return;
    AppEnv *env = _window.lock()->getAppEnvPtr();

    initNoise(time(NULL));

    // Material initialisation
    auto texturedMaterial = std::make_shared<GLS::Material>();
    texturedMaterial->specular = glm::vec3(0.1);
    texturedMaterial->shininess = 64;
    try {
        std::shared_ptr<std::string> textureName = env->getArgument("-texture");
		#ifdef _WIN32 // Just because of the build folder and cmake
        std::string filePath = textureName != nullptr ? *textureName : "../assets/textures/ft_vox_textures.png";
		#else
        std::string filePath = textureName != nullptr ? *textureName : "assets/textures/ft_vox_textures.png";
		#endif
        texturedMaterial->texture_diffuse = std::make_shared<GLS::Texture>(filePath);
        texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        texturedMaterial->texture_mask = texturedMaterial->texture_diffuse;
        texturedMaterial->shininess = 0;
    } catch (std::exception& e) {
        std::cout << "error " << e.what() << std::endl;
    }

    // Lights
    auto pointlightNode = std::make_shared<GLS::Node>();
    auto pointlight = std::make_shared<GLS::Light>();
    pointlight->type = GLS::light_spot;
    pointlight->color = glm::vec3(0.8);
    pointlight->angle *= 1.4;
    pointlightNode->setLight(pointlight);

    auto ambiantlightNode = std::make_shared<GLS::Node>();
    auto ambiantlight = std::make_shared<GLS::Light>();
    ambiantlight->type = GLS::light_ambiant;
    ambiantlight->color = glm::vec3(0.2);
    ambiantlightNode->setLight(ambiantlight);
    scene.rootNode()->addChildNode(ambiantlightNode);

    // Camera
    cameraNode = std::make_shared<GLS::Node>();
    cameraNode->transform().moveBy(5, 10, 20);
    std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
    camera->farZ = 200;
    camera->fogFar = 200;
    camera->fogNear = 100;
    camera->fov = (80.0) * M_PI / 180;
    scene.setBackgroundColor(glm::vec4(0.4, 0.2, 0.1, 1));
    camera->aspect = (scene.getAspect());
    cameraNode->setCamera(camera);
    cameraNode->transform().moveBy(0, 100, 150);
    cameraNode->transform().rotateEulerAnglesBy(-1.2, 0, 0);
    scene.setCameraNode(cameraNode);
    scene.rootNode()->addChildNode(cameraNode);
    cameraNode->addChildNode(pointlightNode);

    // World
    worldNode = std::make_shared<GLS::Node>();
    scene.rootNode()->addChildNode(worldNode);
    _dynamicWorld = std::make_shared<DynamicWorld>(worldNode, "world");
    _dynamicWorld->_generator->usedMaterial = texturedMaterial;

    // Skybox
    std::vector<std::string> skyboxFaces;
    skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_ft.jpg");
    skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_bk.jpg");
    skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_up.jpg");
    skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_dn.jpg");
    skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_rt.jpg");
    skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_lf.jpg");
    try {
        std::shared_ptr<GLS::Skybox> skybox = std::make_shared<GLS::Skybox>(skyboxFaces);
        scene.setSkybox(skybox);
    } catch (std::exception& e) {
        std::cout << "can't load skybox textures with exception: " << e.what() << std::endl;
    }

    _dynamicWorld->_loadedChunks[std::make_pair<int, int>(0, 0)] = _dynamicWorld->_generator->generateBigChunkAt(glm::vec3(4));
    _dynamicWorld->_loadedChunks[std::make_pair<int, int>(1, 0)] = _dynamicWorld->_generator->generateBigChunkAt(glm::vec3(1));
    _dynamicWorld->_loadedChunks[std::make_pair<int, int>(0, 1)] = _dynamicWorld->_generator->generateBigChunkAt(glm::vec3(2));
    _dynamicWorld->_loadedChunks[std::make_pair<int, int>(1, 1)] = _dynamicWorld->_generator->generateBigChunkAt(glm::vec3(3));

    std::shared_ptr<BigChunk> chunk00 = _dynamicWorld->_loadedChunks[std::make_pair<int, int>(0, 0)];
    std::shared_ptr<BigChunk> chunk01 = _dynamicWorld->_loadedChunks[std::make_pair<int, int>(0, 1)];
    std::shared_ptr<BigChunk> chunk10 = _dynamicWorld->_loadedChunks[std::make_pair<int, int>(1, 0)];
    std::shared_ptr<BigChunk> chunk11 = _dynamicWorld->_loadedChunks[std::make_pair<int, int>(1, 1)];

    chunk00->setAdjacentBigChunk(chunk10, 0);
    chunk00->setAdjacentBigChunk(chunk01, 2);

    chunk10->setAdjacentBigChunk(chunk00, 1);
    chunk10->setAdjacentBigChunk(chunk11, 2);

    chunk01->setAdjacentBigChunk(chunk11, 0);
    chunk01->setAdjacentBigChunk(chunk00, 3);

    chunk11->setAdjacentBigChunk(chunk01, 1);
    chunk11->setAdjacentBigChunk(chunk10, 3);

    _dynamicWorld->_loadedChunks[std::make_pair<int, int>(0, 0)]->generateAllMeshes();
    _dynamicWorld->_loadedChunks[std::make_pair<int, int>(0, 0)]->getNode()->transform().setPosition(glm::vec3(0, 0, 0));
    scene.rootNode()->addChildNode(_dynamicWorld->_loadedChunks[std::make_pair<int, int>(0, 0)]->getNode());

    _dynamicWorld->_loadedChunks[std::make_pair<int, int>(1, 0)]->generateAllMeshes();
    _dynamicWorld->_loadedChunks[std::make_pair<int, int>(1, 0)]->getNode()->transform().setPosition(glm::vec3(CHUNKSIZE * BigChunk::bigChunkWidth, 0, 0));
    scene.rootNode()->addChildNode(_dynamicWorld->_loadedChunks[std::make_pair<int, int>(1, 0)]->getNode());

    _dynamicWorld->_loadedChunks[std::make_pair<int, int>(0, 1)]->generateAllMeshes();
    _dynamicWorld->_loadedChunks[std::make_pair<int, int>(0, 1)]->getNode()->transform().setPosition(glm::vec3(0, 0, CHUNKSIZE * BigChunk::bigChunkWidth));
    scene.rootNode()->addChildNode(_dynamicWorld->_loadedChunks[std::make_pair<int, int>(0, 1)]->getNode());

    _dynamicWorld->_loadedChunks[std::make_pair<int, int>(1, 1)]->generateAllMeshes();
    _dynamicWorld->_loadedChunks[std::make_pair<int, int>(1, 1)]->getNode()->transform().setPosition(glm::vec3(CHUNKSIZE * BigChunk::bigChunkWidth, 0, CHUNKSIZE * BigChunk::bigChunkWidth));
    scene.rootNode()->addChildNode(_dynamicWorld->_loadedChunks[std::make_pair<int, int>(1, 1)]->getNode());

}
