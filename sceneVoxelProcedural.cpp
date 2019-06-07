
#include "GLScene.hpp"

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

    static const int worldSize = 64;
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

                        float grassNoise = noise(glm::vec3(0.005) * wpos);
                        chunk->blockAt(cx, noiseHeight, cz) = noise(glm::vec3(0.03) * wpos) * 15 + 3 < 1 ? (grassNoise < 0 ? BLOCK_GRASS_PURPLE : BLOCK_SAND) :
                                                                grassNoise < 0 ? BLOCK_GRASS_BROWN :
                                                                BLOCK_GRASS; // grass
                        if (noiseHeight <= 2) {
                            for (int i = 1; i < 2; i++) {
                                chunk->blockAt(cx, i, cz) = noiseHeight == 1 ? BLOCK_ICE_BROKEN : BLOCK_ICE;
                            }
                        }
                        int h;
                        for (h = 1; h < noiseHeight - 2; h++) {
                            chunk->blockAt(cx, h, cz) = BLOCK_STONE;
                        }
                        for (; h < noiseHeight; h++) {
                            chunk->blockAt(cx, h, cz) = BLOCK_DIRT;
                        }
                        float superNoise = noise(glm::vec3(12.57) * wpos) * noise(glm::vec3(4.5) * wpos) * noise(glm::vec3(0.042) * wpos);
                        if (superNoise >= 0.1 && noiseHeight > 2) {
                            int treeHeight = 5;
                            for (int ht = h + 1; ht < GLS::VoxelChunk::chunkSize && ht < h + treeHeight; ht++) {
                                chunk->blockAt(cx, ht, cz) = BLOCK_WOOD;
                            }
                            for (int ly = h + 3; ly <= h + treeHeight + 1; ly++) {
                                int lr = ly <= h + 4 ? 2 : 1;
                                for (int lx = cx - lr; lx <= cx + lr; lx++) {
                                    for (int lz = cz - lr; lz <= cz + lr; lz++) {
                                        if (lx >= 0 && lx < GLS::VoxelChunk::chunkSize
                                            && lz >= 0 && lz < GLS::VoxelChunk::chunkSize
                                            && ly < GLS::VoxelChunk::chunkSize
                                            && chunk->blockAt(lx, ly, lz) == 0) {
                                            chunk->blockAt(lx, ly, lz) = BLOCK_LEAFS;
                                        }
                                    }
                                }
                            }
                        }
                        for (h++; h < GLS::VoxelChunk::chunkSize; h++) {
                            if (cx * cz + h % 15 == 0)
                                chunk->blockAt(cx, h, cz) = BLOCK_WOOD_PLANKS;
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

static std::shared_ptr<GLS::Node> worldNode = nullptr;
static std::shared_ptr<GLS::Node> cameraNode = nullptr;

void updateSceneVoxelProcedural(double et, double dt) {
    (void)dt;
    static double lt = et;

    if (et - lt >= 0.3 || true) {
        lt = et;
        if (worldNode != nullptr && cameraNode != nullptr) {
            auto camera = cameraNode->camera();
            for (size_t i = 0; i < worldNode->childNodes().size(); i++) {
                std::shared_ptr<GLS::Node> chunkNode = worldNode->childNodeAt(i);
                glm::vec3 chunkPos = chunkNode->transform().position() + glm::vec3(GLS::VoxelChunk::chunkSize / 2);
                glm::vec3 cameraPos = cameraNode->transform().position();
                glm::vec3 chunkDirection = chunkPos - cameraPos;
                bool a = false;
                float length = glm::length(chunkDirection);
                if (length < GLS::VoxelChunk::chunkSize * 1.5)
                    a = true;
                else if (length > 160)
                    a = false;
                else {
                    chunkDirection /= length;
                    glm::vec3 cameraDirection = glm::vec3(cameraNode->transform().matrix() * glm::vec4(0, 0, -1, 0));
                    float cosangle = glm::dot(chunkDirection, cameraDirection);
                    a = cosangle > cos(1.2 * camera->fov * camera->aspect / 2);
                }
                chunkNode->setActive(a);
            }
        }
    }
}

void loadSceneVoxelProcedural(GLS::Scene& scene, const std::vector<std::string>& args) {

    auto texturedMaterial = std::make_shared<GLS::Material>();
    texturedMaterial->specular = glm::vec3(0.1);
    texturedMaterial->shininess = 64;
    try {
        std::string filePath = args.size() >= 1 ? args[0] : "../textures/ft_vox_textures.png";
        texturedMaterial->texture_diffuse = std::make_shared<GLS::Texture>(filePath);
        texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        texturedMaterial->texture_mask = texturedMaterial->texture_diffuse;
        texturedMaterial->shininess = 0;
        if (args.size() >= 2)
            texturedMaterial->texture_normal = std::make_shared<GLS::Texture>(args[1]);
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
    camera->fov = (80.0) * M_PI / 180;
    camera->aspect = (scene.getAspect());
    cameraNode->setCamera(camera);
    cameraNode->transform().moveBy(0, 2, 5);
    cameraNode->transform().rotateEulerAnglesBy(0.1, 0.1, 0);
    scene.setCameraNode(cameraNode);
    scene.rootNode()->addChildNode(cameraNode);
    cameraNode->addChildNode(pointlightNode);

    worldNode = std::make_shared<GLS::Node>();
    VoxelWorld world(texturedMaterial);
    world.embedInNode(worldNode);
    scene.rootNode()->addChildNode(worldNode);
}
