
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

double noise(glm::vec3 v) {
    return noise(v.x, v.y, v.z);
}

#define CHUNKSIZE GLS::VoxelChunk::chunkSize

class VoxelWorld {

    static const int worldSize = 16;
    static const int worldHeight = 32 / CHUNKSIZE;
    typedef std::array<std::array<std::array<std::shared_ptr<GLS::VoxelChunk>, worldSize>, worldHeight>, worldSize> VoxelMap;
    VoxelMap _voxels;

    public:

    VoxelWorld(std::shared_ptr<GLS::Material> mat = nullptr) : _voxels() {
        for (int wx = 0; wx < worldSize; wx++)
            for (int wy = 0; wy < worldHeight; wy++)
                for (int wz = 0; wz < worldSize; wz++) {
                    std::shared_ptr<GLS::VoxelChunk> chunk = std::make_shared<GLS::VoxelChunk>();
                    chunk->setMaterial(mat);
                    _voxels[wx][wy][wz] = chunk;
                } // wz

        std::weak_ptr<GLS::VoxelChunk> nochunk;
        for (int x = 0; x < worldSize; x++)
            for (int y = 0; y < worldHeight; y++)
                for (int z = 0; z < worldSize; z++) {
                    std::array<std::weak_ptr<GLS::VoxelChunk>, 6> adjacents = {{
                        x == worldSize - 1 ? nochunk : _voxels[x + 1][y][z],
                        x == 0 ? nochunk : _voxels[x - 1][y][z],
                        y == worldHeight - 1 ? nochunk : _voxels[x][y + 1][z],
                        y == 0 ? nochunk : _voxels[x][y - 1][z],
                        z == worldSize - 1 ? nochunk : _voxels[x][y][z + 1],
                        z == 0 ? nochunk : _voxels[x][y][z - 1]
                    }};
                    _voxels[x][y][z]->setAdjacentChunks(adjacents);
                }
    }

    void generate() {

        for (int wx = 0; wx < worldSize; wx++)
            for (int wy = 0; wy < worldHeight; wy++)
                for (int wz = 0; wz < worldSize; wz++) {

                    // Generation of one chunk
                    std::shared_ptr<GLS::VoxelChunk> chunk = _voxels[wx][wy][wz];
                    for (int cx = 0; cx < CHUNKSIZE; cx++)
                        for (int cz = 0; cz < CHUNKSIZE; cz++) {
                            glm::vec3 wpos(wx * CHUNKSIZE + cx,
                                            wy * CHUNKSIZE,
                                            wz * CHUNKSIZE + cz);

                            // Generation Bedrock
                            int h = 0;
                            if (wy == 0) {
                                chunk->setBlockIdAt(cx, h++, cz, BLOCK_BEDROCK, true);
                                for (; h < 4; h++) {
                                    float nValue = noise(wpos.x * 1.2454, h + 5.24, wpos.z * 4.6378);
                                    chunk->setBlockIdAt(cx, h, cz, nValue >= 0.1 * h ? BLOCK_BEDROCK : BLOCK_STONE);
                                }
                            }

                            // Calcul Biome
                            const int biomeCount = 5;
                            const int biomeBlocks[biomeCount][2] = {
                                {BLOCK_DIRT, BLOCK_GRASS},
                                {BLOCK_DIRT, BLOCK_GRASS_BROWN},
                                {BLOCK_SAND, BLOCK_SAND},
                                {BLOCK_DIRT, BLOCK_GRASS},
                                {BLOCK_ICE, BLOCK_ICE_BROKEN}
                            };
                            float biomeCoefficients[biomeCount]; // [plane, forest, desert, mountains, ice]
                            int biomeBlock[2];
                            int biomeIndex = 0;
                            {
                                float totalValues = 0;
                                for (int i = 0; i < biomeCount; i++) {
                                    float nValue = noise(glm::vec3(1549 * i + 436, 0.23, 6134 * i + 2613) + wpos * glm::vec3(0.001, 0, 0.001) * glm::vec3(2));
                                    biomeCoefficients[i] = nValue * nValue;
                                    totalValues += biomeCoefficients[i];
                                    if (biomeCoefficients[i] > biomeCoefficients[biomeIndex])
                                        biomeIndex = i;
                                }
                                biomeBlock[0] = biomeBlocks[biomeIndex][0];
                                biomeBlock[1] = biomeBlocks[biomeIndex][1];
                                for (int i = 0; i < biomeCount; i++) {
                                    biomeCoefficients[i] = biomeCoefficients[i] / totalValues;
                                }
                            }

                            // Calcul ground height
                            int groundHeight = 0;
                            {
                                float groundValue = 0
                                    + 2.5 * noise(wpos * glm::vec3(0.027, 0, 0.024)) * biomeCoefficients[0]
                                    + 4.6 * noise(wpos * glm::vec3(0.046, 0, 0.073)) * biomeCoefficients[1]
                                    + 12.5 * noise(wpos * glm::vec3(0.05, 0, 0.05)) * noise(wpos * glm::vec3(0.05, 0, 0.05)) * biomeCoefficients[2] + 22 * biomeCoefficients[2]
                                    + 150.6 * noise(wpos * glm::vec3(0.015, 0, 0.034)) * noise(wpos * glm::vec3(0.015, 0, 0.034)) * biomeCoefficients[3] + 64 * biomeCoefficients[3]
                                    + 65.6 * noise(wpos * glm::vec3(0.08, 0, 0.08)) * noise(wpos * glm::vec3(0.08, 0, 0.08)) * noise(wpos * glm::vec3(0.08, 0, 0.08)) * biomeCoefficients[4]
                                ;
                                groundHeight = groundValue + 50;
                            }

                            // Snow height
                            int snowHeight = 0;
                            {
                                float nValue = noise(wpos * glm::vec3(0.035, 0, 0.33));
                                snowHeight = nValue * 8 + 110;
                            }

                            // Generate Caves
                            for (; h <= groundHeight && h < CHUNKSIZE; h++) {
                                float nValue = noise(glm::vec3(0.03, 0.12, 0.04) * (wpos + glm::vec3(0, h, 0)));
                                int caveBlockValue = nValue > -0.40 ? BLOCK_STONE :
                                                    nValue > -0.41 ? BLOCK_COBBLESTONE :
                                                    BLOCK_AIR;

                                int wh = wpos.y + h;
                                int groundBlockValue = wh > groundHeight ? BLOCK_AIR :
                                                    wh > snowHeight ? BLOCK_ICE :
                                                    wh == groundHeight ? biomeBlock[1] :
                                                    wh > groundHeight - 3 ? biomeBlock[0] :
                                                    BLOCK_STONE;

                                int blockValue = caveBlockValue == BLOCK_AIR ? BLOCK_AIR :
                                                    wh > groundHeight - 3 ? groundBlockValue :
                                                    caveBlockValue;

                                chunk->setBlockIdAt(cx, h, cz, blockValue);
                            }

                            // for (h++; h < CHUNKSIZE; h++) {
                            //     if (cx * cz * h % 15 == 0)
                            //         chunk->setBlockIdAt(cx, h, cz, BLOCK_WOOD_PLANKS);
                            // }
                        }

                }

        for (int x = 0; x < worldSize; x++)
            for (int y = 0; y < worldHeight; y++)
                for (int z = 0; z < worldSize; z++) {
                    _voxels[x][y][z]->calculBlockAdjacence();
                }

    }

    void embedInNode(std::shared_ptr<GLS::Node> worldNode) {
        for (int x = 0; x < worldSize; x++) {
            for (int y = 0; y < worldHeight; y++) {
                for (int z = 0; z < worldSize; z++) {
                    std::shared_ptr<GLS::Node> chunkNode = std::make_shared<GLS::Node>();
                    float magni = 1.0 * CHUNKSIZE;
                    float worldRay = (worldSize * CHUNKSIZE * 1.0) / 2.0;
                    glm::vec3 offset(-worldRay, -CHUNKSIZE / 2, -worldRay);
                    chunkNode->transform().setPosition(glm::vec3(x * magni, y * magni, z * magni) + offset);
                    chunkNode->addRenderable(GLS::Mesh::voxelChunk(_voxels[x][y][z]));
                    worldNode->addChildNode(chunkNode);
                }
            }
        }
    }

};

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
        if (worldNode != nullptr && cameraNode != nullptr) {
            auto camera = cameraNode->camera();
            for (size_t i = 0; i < worldNode->childNodes().size(); i++) {
                std::shared_ptr<GLS::Node> chunkNode = worldNode->childNodeAt(i);
                glm::vec3 chunkPos = chunkNode->transform().position() + glm::vec3(CHUNKSIZE / 2);
                glm::vec3 cameraPos = cameraNode->transform().position();
                glm::vec3 chunkDirection = chunkPos - cameraPos;
                bool a = false;
                float length = glm::length(chunkDirection);
                if (length < CHUNKSIZE * 1.5)
                    a = true;
                else if (length > camera->farZ)
                    a = false;
                else {
                    chunkDirection /= length;
                    glm::vec3 cameraDirection = glm::vec3(cameraNode->transform().matrix() * glm::vec4(0, 0, -1, 0));
                    float cosangle = glm::dot(chunkDirection, cameraDirection);
                    a = cosangle > cos(1.3 * camera->fov * camera->aspect / 2);
                }
                chunkNode->setActive(a);
            }
        }
    }
}

void VoxelProceduralSceneController::makeScene() {

    GLS::Scene& scene(*_scene);
    if (_window.expired())
        return;
    AppEnv *env = _window.lock()->getAppEnvPtr();

    initNoise(time(NULL));

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

    cameraNode = std::make_shared<GLS::Node>();
    cameraNode->transform().moveBy(5, 10, 20);
    std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
    camera->farZ = 300;
    camera->fogFar = 300;
    camera->fogNear = 100;
    camera->fov = (80.0) * M_PI / 180;
    camera->aspect = (scene.getAspect());
    cameraNode->setCamera(camera);
    cameraNode->transform().moveBy(0, 100, 150);
    cameraNode->transform().rotateEulerAnglesBy(-1.2, 0, 0);
    scene.setCameraNode(cameraNode);
    scene.rootNode()->addChildNode(cameraNode);
    cameraNode->addChildNode(pointlightNode);

    worldNode = std::make_shared<GLS::Node>();
    VoxelWorld world(texturedMaterial);
    world.generate();
    world.embedInNode(worldNode);
    scene.rootNode()->addChildNode(worldNode);

}
