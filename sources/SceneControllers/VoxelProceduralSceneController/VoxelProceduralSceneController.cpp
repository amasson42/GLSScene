
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

double linearNoise(glm::vec3 v) {
    return linearNoise(v.x, v.y, v.z);
}

namespace glm {
    bool operator<(const glm::ivec2& lhs, const glm::ivec2& rhs) {
        if (lhs.x < rhs.x)
            return true;
        else
            return lhs.y < rhs.y;
    }
}

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
    static const int bigChunkWidth = 2;
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
                    chunk.node->setName("VX_" + std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z));
                    chunk.mustUpdateMesh = true;
                    _node->addChildNode(chunk.node);
                }
            }
        }
    }

    GameVoxelChunk& chunkAt(int i) {
        if (i < 0 || i >= bigChunkCount)
            throw std::runtime_error("fuck off");
        return _chunks[i];
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
    void setAdjacentBigChunk_positiveX(std::shared_ptr<BigChunk> adj) {
        _adjacents[0] = adj;
        const int bcs = bigChunkWidth - 1;
        int x = bcs;
        for (int y = 0; y < bigChunkHeight; y++) {
            for (int z = 0; z < bigChunkWidth; z++) {
                GameVoxelChunk& chunk(chunkAt(x, y, z));
                chunk.voxel->setAdjacentChunk(adj->chunkAt(0, y, z).voxel, 0);
                chunk.voxel->calculBlockAdjacenceEdge(0);
                chunk.mustUpdateMesh = true;
            }
        }
    }

    void setAdjacentBigChunk_negativeX(std::shared_ptr<BigChunk> adj) {
        _adjacents[1] = adj;
        const int bcs = bigChunkWidth - 1;
        int x = 0;
        for (int y = 0; y < bigChunkHeight; y++) {
            for (int z = 0; z < bigChunkWidth; z++) {
                GameVoxelChunk& chunk(chunkAt(x, y, z));
                chunk.voxel->setAdjacentChunk(adj->chunkAt(bcs, y, z).voxel, 1);
                chunk.voxel->calculBlockAdjacenceEdge(1);
                chunk.mustUpdateMesh = true;
            }
        }
    }

    void setAdjacentBigChunk_positiveZ(std::shared_ptr<BigChunk> adj) {
        _adjacents[2] = adj;
        const int bcs = bigChunkWidth - 1;
        for (int x = 0; x < bigChunkWidth; x++) {
            for (int y = 0; y < bigChunkHeight; y++) {
                int z = bcs;
                GameVoxelChunk& chunk(chunkAt(x, y, z));
                chunk.voxel->setAdjacentChunk(adj->chunkAt(x, y, 0).voxel, 4);
                chunk.voxel->calculBlockAdjacenceEdge(4);
                chunk.mustUpdateMesh = true;
            }
        }
    }

    void setAdjacentBigChunk_negativeZ(std::shared_ptr<BigChunk> adj) {
        _adjacents[3] = adj;
        const int bcs = bigChunkWidth - 1;
        for (int x = 0; x < bigChunkWidth; x++) {
            for (int y = 0; y < bigChunkHeight; y++) {
                int z = 0;
                GameVoxelChunk& chunk(chunkAt(x, y, z));
                chunk.voxel->setAdjacentChunk(adj->chunkAt(x, y, bcs).voxel, 5);
                chunk.voxel->calculBlockAdjacenceEdge(5);
                chunk.mustUpdateMesh = true;
            }
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

    std::shared_ptr<BigChunk> generateBigChunkAt(glm::ivec2 bigChunkPos) {
        std::shared_ptr<BigChunk> bc = std::make_shared<BigChunk>(usedMaterial);

        for (int cx = 0; cx < BigChunk::bigChunkWidth; cx++) {
            for (int cy = 0; cy < BigChunk::bigChunkHeight; cy++) {
                for (int cz = 0; cz < BigChunk::bigChunkWidth; cz++) {

                    glm::vec3 chunk_wpos = glm::vec3(bigChunkPos.x * CHUNKSIZE * BigChunk::bigChunkWidth + cx * CHUNKSIZE,
                                                     cy * CHUNKSIZE,
                                                     bigChunkPos.y * CHUNKSIZE * BigChunk::bigChunkWidth + cz * CHUNKSIZE);

                    std::shared_ptr<GLS::VoxelChunk> voxel = bc->chunkAt(cx, cy, cz).voxel;

                    for (int x = 0; x < CHUNKSIZE; x++) {
                        for (int z = 0; z < CHUNKSIZE; z++) {
                            glm::vec3 wpos = chunk_wpos + glm::vec3(x, 0, z);

                            int h = 0;
                            if (cy == 0) {
                                voxel->setBlockIdAt(x, h++, z, BLOCK_BEDROCK);
                                for (; h < 4; h++) {
                                    float nValue = linearNoise(wpos.x * 1.2454, h + 5.24, wpos.z * 4.6378);
                                    voxel->setBlockIdAt(x, h, z, nValue >= 0.1 * h ? BLOCK_BEDROCK : BLOCK_STONE);
                                }
                            }

                            int groundHeight = 4.6 * linearNoise(wpos * glm::vec3(0.046, 0, 0.073)) + 50;

                            int snowHeight = 0;
                            {
                                float nValue = linearNoise(wpos * glm::vec3(0.035, 0, 0.33));
                                snowHeight = nValue * 8 + 110;
                            }

                            for (; h <= groundHeight && h < CHUNKSIZE; h++) {
                                float nValue = linearNoise(glm::vec3(0.03, 0.12, 0.04) * (wpos + glm::vec3(0, h, 0)));
                                int caveBlockValue = nValue > -0.40 ? BLOCK_STONE :
                                                    nValue > -0.41 ? BLOCK_COBBLESTONE :
                                                    BLOCK_AIR;

                                int wh = wpos.y + h;
                                int groundBlockValue = wh > groundHeight ? BLOCK_AIR :
                                                    wh > snowHeight ? BLOCK_ICE :
                                                    wh == groundHeight ? BLOCK_GRASS :
                                                    wh > groundHeight - 3 ? BLOCK_DIRT :
                                                    BLOCK_STONE;

                                int blockValue = caveBlockValue == BLOCK_AIR ? BLOCK_AIR :
                                                    wh > groundHeight - 3 ? groundBlockValue :
                                                    caveBlockValue;

                                voxel->setBlockIdAt(x, h, z, blockValue);
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

    std::map<glm::ivec2, std::shared_ptr<BigChunk> > _loadedChunks;
    std::set<glm::ivec2> _loadingChunks;

    // world/
    //  info.json -> {"seed": 421337}
    //  C_X_Z.chunk
    //  C_[...]_[...].chunk
    DynamicWorld(std::shared_ptr<GLS::Node> worldNode, std::string worldName) :
    _loadedChunks() {
        _worldNode = worldNode;
        _worldDirName = worldName;
        _generator = std::make_shared<ProceduralWorldGenerator>();
    }

    glm::ivec2 worldToBigChunkPosition(glm::vec3 position) {
        return glm::ivec2(position.x / (BigChunk::bigChunkWidth * CHUNKSIZE),
                          position.z / (BigChunk::bigChunkWidth * CHUNKSIZE));
    }

    glm::vec3 bigChunkPositionToWorld(glm::ivec2 position) {
        return glm::vec3(position.x * BigChunk::bigChunkWidth * CHUNKSIZE,
                         0,
                         position.y * BigChunk::bigChunkWidth * CHUNKSIZE);
    }

    /**
     * the method called every frame
     * send the position of the player
     * will load the unloaded chunks near enough
     * will unload the loaded chunks far enough
     */
    void loadPosition(std::shared_ptr<GLS::Node> cameraNode) {

        float loadingDistance = 120.0;
        float visibleDistance = 100;

        std::shared_ptr<GLS::Camera> camera = cameraNode->camera();
        if (camera == nullptr)
            return;
        float minCosCameraVision = cos(1.4 * camera->fov * camera->aspect / 2);

        glm::vec3 chunkMid = glm::vec3(CHUNKSIZE * BigChunk::bigChunkWidth / 2,
                                       0,
                                       CHUNKSIZE * BigChunk::bigChunkWidth / 2);

        glm::vec3 cameraFlatPosition = cameraNode->transform().position();
        cameraFlatPosition.y = 0;

        std::map<glm::ivec2, std::shared_ptr<BigChunk> >::iterator it = _loadedChunks.begin();
        while (it != _loadedChunks.end()) {
            glm::vec3 chunkOffset = (it->second->getNode()->transform().position() + chunkMid) - cameraFlatPosition;
            if (glm::dot(chunkOffset, chunkOffset) > loadingDistance * loadingDistance) {

                // TODO: save and destroy the chunk
                std::cout << "remove node " << std::endl;
                it->second->getNode()->removeFromParent();

                it = _loadedChunks.erase(it);
            } else {

                // std::shared_ptr<BigChunk> bigChunk = it->second;
                // for (int i = 0; i < BigChunk::bigChunkCount; i++) {
                //     GameVoxelChunk& chunk(bigChunk->chunkAt(i));
                //     if (chunk.mustUpdateMesh) {
                //         chunk.updateMesh();
                //     }
                //     glm::vec3 chunkWorldPos = bigChunk->getNode()->transform().position() + chunk.node->transform().position() + glm::vec3(CHUNKSIZE / 2);
                //     glm::vec3 cameraPos = cameraNode->transform().position();
                //     glm::vec3 chunkDirection = chunkWorldPos - cameraPos;
                //     float squaredLength = glm::dot(chunkDirection, chunkDirection);
                //     if (squaredLength < visibleDistance * visibleDistance) {
                //         if (squaredLength < 1.75 * CHUNKSIZE) {
                //             chunk.node->setActive(true);
                //         } else {
                //             chunkDirection = glm::normalize(chunkDirection);
                //             glm::vec3 cameraDirection = glm::vec3(cameraNode->transform().matrix() * glm::vec4(0, 0, -1, 0));
                //             float cosangle = glm::dot(chunkDirection, cameraDirection);
                //             chunk.node->setActive(cosangle > minCosCameraVision);
                //         }
                //     } else {
                //         chunk.node->setActive(false);
                //     }
                // }

                ++it;
            }

        }

        glm::ivec2 minPosition = worldToBigChunkPosition(cameraFlatPosition - glm::vec3(loadingDistance, 0, loadingDistance));
        glm::ivec2 maxPosition = worldToBigChunkPosition(cameraFlatPosition + glm::vec3(loadingDistance, 0, loadingDistance));

        for (int x = minPosition.x; x < maxPosition.x; x++) {
            for (int y = minPosition.y; y < maxPosition.y; y++) {

                if (_loadedChunks.find(glm::ivec2(x, y)) != _loadedChunks.end())
                    continue;

                glm::vec3 chunkOffset = bigChunkPositionToWorld(glm::ivec2(x, y)) - cameraFlatPosition;
                if (glm::dot(chunkOffset, chunkOffset) < loadingDistance * loadingDistance) {

                    if (_loadingChunks.find(glm::ivec2(x, y)) == _loadingChunks.end()) {
                        _loadingChunks.insert(glm::ivec2(x, y));

                        {   // TODO: do this in a thread
                            // TODO: load from a file if possible
                            std::shared_ptr<BigChunk> generatedChunk = _generator->generateBigChunkAt(glm::ivec2(x, y));
                            generatedChunk->getNode()->transform().setPosition(bigChunkPositionToWorld(glm::ivec2(x, y)));

                            _loadedChunks[glm::ivec2(x, y)] = generatedChunk;
                            _worldNode->addChildNode(generatedChunk->getNode());

                            // define adjacent
                            auto adjacent = _loadedChunks.begin();

                            adjacent = _loadedChunks.find(glm::ivec2(x + 1, y));
                            if (adjacent != _loadedChunks.end()) {
                                generatedChunk->setAdjacentBigChunk_positiveX(adjacent->second);
                                adjacent->second->setAdjacentBigChunk_negativeX(generatedChunk);
                            }

                            adjacent = _loadedChunks.find(glm::ivec2(x - 1, y));
                            if (adjacent != _loadedChunks.end()) {
                                generatedChunk->setAdjacentBigChunk_negativeX(adjacent->second);
                                adjacent->second->setAdjacentBigChunk_positiveX(generatedChunk);
                            }

                            adjacent = _loadedChunks.find(glm::ivec2(x, y + 1));
                            if (adjacent != _loadedChunks.end()) {
                                generatedChunk->setAdjacentBigChunk_positiveZ(adjacent->second);
                                adjacent->second->setAdjacentBigChunk_negativeZ(generatedChunk);
                            }

                            adjacent = _loadedChunks.find(glm::ivec2(x, y - 1));
                            if (adjacent != _loadedChunks.end()) {
                                generatedChunk->setAdjacentBigChunk_negativeZ(adjacent->second);
                                adjacent->second->setAdjacentBigChunk_positiveZ(generatedChunk);
                            }

                            generatedChunk->generateAllMeshes();

                            _loadingChunks.erase(glm::ivec2(x, y));
                        }

                    }

                }

            }
        }

        it = _loadedChunks.begin();
        while (it != _loadedChunks.end()) {
            std::shared_ptr<BigChunk> bigChunk = it->second;
            for (int i = 0; i < BigChunk::bigChunkCount; i++) {
                GameVoxelChunk& chunk(bigChunk->chunkAt(i));
                if (chunk.mustUpdateMesh) {
                    chunk.updateMesh();
                }
                glm::vec3 chunkWorldPos = bigChunk->getNode()->transform().position() + chunk.node->transform().position() + glm::vec3(CHUNKSIZE / 2);
                glm::vec3 cameraPos = cameraNode->transform().position();
                glm::vec3 chunkDirection = chunkWorldPos - cameraPos;
                float squaredLength = glm::dot(chunkDirection, chunkDirection);
                if (squaredLength < visibleDistance * visibleDistance) {
                    if (squaredLength < 1.75 * CHUNKSIZE) {
                        chunk.node->setActive(true);
                    } else {
                        chunkDirection = glm::normalize(chunkDirection);
                        glm::vec3 cameraDirection = glm::vec3(cameraNode->transform().matrix() * glm::vec4(0, 0, -1, 0));
                        float cosangle = glm::dot(chunkDirection, cameraDirection);
                        chunk.node->setActive(cosangle > minCosCameraVision);
                    }
                } else {
                    chunk.node->setActive(false);
                }
            }

            ++it;
        }
        std::cout << "We have " << _loadedChunks.size() << " chunks" << std::endl;

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
    static int doneTimes = 0;

    if (doneTimes < 2 && et - lt >= 0.2) {
        lt = et;
        // _dynamicWorld->loadPosition(cameraNode);
        // _scene->rootNode()->sendToFlux(std::cout, "");
        doneTimes++;
    }
}

void VoxelProceduralSceneController::keyCallBack(int key, int scancode, int action, int mods) {
    ISceneController::keyCallBack(key, scancode, action, mods);
    if (key == 84 && action == 1) {
        _dynamicWorld->loadPosition(cameraNode);
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
    cameraNode->transform().moveBy(0, 10, 16);
    std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
    camera->farZ = 200;
    camera->fogFar = 200;
    camera->fogNear = 100;
    camera->fov = (80.0) * M_PI / 180;
    scene.setBackgroundColor(glm::vec4(0.4, 0.2, 0.1, 1));
    camera->aspect = (scene.getAspect());
    cameraNode->setCamera(camera);
    // cameraNode->transform().rotateEulerAnglesBy(-1.2, 0, 0);
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

    // _dynamicWorld->_loadedChunks[glm::ivec2(10, 0)] = _dynamicWorld->_generator->generateBigChunkAt(glm::vec3(4));
    // _dynamicWorld->_loadedChunks[glm::ivec2(11, 0)] = _dynamicWorld->_generator->generateBigChunkAt(glm::vec3(1));
    // _dynamicWorld->_loadedChunks[glm::ivec2(10, 1)] = _dynamicWorld->_generator->generateBigChunkAt(glm::vec3(2));
    // _dynamicWorld->_loadedChunks[glm::ivec2(11, 1)] = _dynamicWorld->_generator->generateBigChunkAt(glm::vec3(3));

    // std::shared_ptr<BigChunk> chunk00 = _dynamicWorld->_loadedChunks[glm::ivec2(10, 0)];
    // std::shared_ptr<BigChunk> chunk01 = _dynamicWorld->_loadedChunks[glm::ivec2(10, 1)];
    // std::shared_ptr<BigChunk> chunk10 = _dynamicWorld->_loadedChunks[glm::ivec2(11, 0)];
    // std::shared_ptr<BigChunk> chunk11 = _dynamicWorld->_loadedChunks[glm::ivec2(11, 1)];

    // _dynamicWorld->_loadedChunks[glm::ivec2(10, 0)]->generateAllMeshes();
    // _dynamicWorld->_loadedChunks[glm::ivec2(10, 0)]->getNode()->transform().setPosition(glm::vec3(0, 0, 0));
    // scene.rootNode()->addChildNode(_dynamicWorld->_loadedChunks[glm::ivec2(10, 0)]->getNode());

    // _dynamicWorld->_loadedChunks[glm::ivec2(11, 0)]->generateAllMeshes();
    // _dynamicWorld->_loadedChunks[glm::ivec2(11, 0)]->getNode()->transform().setPosition(glm::vec3(CHUNKSIZE * BigChunk::bigChunkWidth, 0, 0));
    // scene.rootNode()->addChildNode(_dynamicWorld->_loadedChunks[glm::ivec2(11, 0)]->getNode());

    // _dynamicWorld->_loadedChunks[glm::ivec2(10, 1)]->generateAllMeshes();
    // _dynamicWorld->_loadedChunks[glm::ivec2(10, 1)]->getNode()->transform().setPosition(glm::vec3(0, 0, CHUNKSIZE * BigChunk::bigChunkWidth));
    // scene.rootNode()->addChildNode(_dynamicWorld->_loadedChunks[glm::ivec2(10, 1)]->getNode());

    // _dynamicWorld->_loadedChunks[glm::ivec2(11, 1)]->generateAllMeshes();
    // _dynamicWorld->_loadedChunks[glm::ivec2(11, 1)]->getNode()->transform().setPosition(glm::vec3(CHUNKSIZE * BigChunk::bigChunkWidth, 0, CHUNKSIZE * BigChunk::bigChunkWidth));
    // scene.rootNode()->addChildNode(_dynamicWorld->_loadedChunks[glm::ivec2(11, 1)]->getNode());

}
