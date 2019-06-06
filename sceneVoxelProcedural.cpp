
#include "GLScene.hpp"

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
#define BLOCK_GOLD 45
#define BLOCK_TNT 92

void initNoise();
double noise(double x, double y, double z);
double noise(glm::vec3 v) {
    static bool first = true;
    if (first) {
        initNoise();
        first = false;
    }
    return noise(v.x, v.y, v.z);
}

class VoxelWorld {

    static const int worldSize = 2;
    typedef std::array<std::array<std::shared_ptr<GLS::VoxelChunk>, worldSize>, worldSize> VoxelMap;
    VoxelMap _voxels;

    public:

    VoxelWorld(std::shared_ptr<GLS::Material> mat = nullptr) : _voxels() {
        for (int wx = 0; wx < worldSize; wx++) {
            for (int wz = 0; wz < worldSize; wz++) {
                std::shared_ptr<GLS::VoxelChunk> chunk = std::make_shared<GLS::VoxelChunk>();
                for (int cx = 0; cx < GLS::VoxelChunk::chunkSize; cx++)
                    for (int cz = 0; cz < GLS::VoxelChunk::chunkSize; cz++) {
                        glm::vec3 wpos(wx * GLS::VoxelChunk::chunkSize + cx,
                                        0,
                                        wz * GLS::VoxelChunk::chunkSize + cz);
                        chunk->blockAt(cx, 0, cz) = BLOCK_BEDROCK;
                        int noiseHeight = noise(glm::vec3(0.03) * wpos) * 15 + 3;
                        noiseHeight = noiseHeight <= 0 ? 1 - noiseHeight : noiseHeight;
                        noiseHeight = noiseHeight > GLS::VoxelChunk::chunkSize ? 1 : noiseHeight;
                        chunk->blockAt(cx, noiseHeight, cz) = BLOCK_GRASS;
                        int h;
                        for (h = 1; h < noiseHeight - 2; h++) {
                            chunk->blockAt(cx, h, cz) = BLOCK_STONE;
                        }
                        for (; h < noiseHeight; h++) {
                            chunk->blockAt(cx, h, cz) = BLOCK_DIRT;
                        }
                    }
                chunk->setMaterial(mat);
                _voxels[wx][wz] = chunk;
            }
        }
        for (int x = 0; x < worldSize; x++)
            for (int z = 0; z < worldSize; z++) {
                _voxels[x][z]->calculBlockAdjacence({
                    x == worldSize - 1 ? nullptr : _voxels[x + 1][z],
                    x == 0 ? nullptr : _voxels[x - 1][z],
                    nullptr, nullptr,
                    z == worldSize - 1 ? nullptr : _voxels[x][z + 1],
                    z == 0 ? nullptr : _voxels[x][z - 1]});
                _voxels[x][z]->generateBuffers();
            }
    }

    void embedInNode(std::shared_ptr<GLS::Node> worldNode) {
        for (int x = 0; x < worldSize; x++) {
            for (int z = 0; z < worldSize; z++) {
                std::shared_ptr<GLS::Node> chunkNode = std::make_shared<GLS::Node>();
                float magni = 1.0 * GLS::VoxelChunk::chunkSize;
                float worldRay = (worldSize * GLS::VoxelChunk::chunkSize * 1.0) / 2.0;
                glm::vec3 offset(-worldRay, -GLS::VoxelChunk::chunkSize / 2, -worldRay);
                chunkNode->transform().setPosition(glm::vec3(x * magni, 0, z * magni) + offset);
                chunkNode->addRenderable(_voxels[x][z]);
                worldNode->addChildNode(chunkNode);
            }
        }
    }

};

void loadSceneVoxelProcedural(GLS::Scene& scene, const std::vector<std::string>& args) {

    auto texturedMaterial = std::make_shared<GLS::Material>();
    texturedMaterial->specular = glm::vec3(0.1);
    texturedMaterial->shininess = 64;
    try {
        std::string filePath = args.size() >= 1 ? args[0] : "../textures/ft_vox_textures.png";
        texturedMaterial->texture_diffuse = std::make_shared<GLS::Texture>(filePath);
        texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        if (args.size() >= 2)
            texturedMaterial->texture_normal = std::make_shared<GLS::Texture>(args[1]);
    } catch (std::exception& e) {
        std::cout << "error " << e.what() << std::endl;
    }

    auto pointlightNode = std::make_shared<GLS::Node>();
    auto pointlight = std::make_shared<GLS::Light>();
    pointlight->type = GLS::light_spot;
    pointlight->color = glm::vec3(1.5);
    pointlight->angle *= 1.4;
    pointlightNode->setLight(pointlight);

    std::shared_ptr<GLS::Node> cameraNode = std::make_shared<GLS::Node>();
    cameraNode->transform().moveBy(5, 10, 20);
    std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
    camera->aspect = (scene.getAspect());
    cameraNode->setCamera(camera);
    cameraNode->transform().moveBy(0, 2, 5);
    scene.setCameraNode(*cameraNode);
    scene.rootNode().addChildNode(cameraNode);
    cameraNode->addChildNode(pointlightNode);

    std::shared_ptr<GLS::Node> worldNode = std::make_shared<GLS::Node>();
    VoxelWorld world(texturedMaterial);
    world.embedInNode(worldNode);
    scene.rootNode().addChildNode(worldNode);
}
