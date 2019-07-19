
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

struct BigChunk {
    
};

/**
 * the class that is able to generate chunks with cool environnements
 */
class ProceduralWorldGenerator {



    public:

    ProceduralWorldGenerator() {

    }

    std::shared_ptr<BigChunk> generateBigChunkAt(glm::vec3 wpos);

};

/**
 * the class that will manage chunk loading/saving and all those shits
 */
class DynamicWorld {

    std::string _worldDirName;
    std::shared_ptr<GLS::Node> _worldNode;
    std::shared_ptr<ProceduralWorldGenerator> _generator;

    std::map<std::pair<int, int>, std::shared_ptr<BigChunk> > _loadedChunks;

    public:

    DynamicWorld(std::shared_ptr<GLS::Node> worldNode, std::string worldName) {
        _worldNode = worldNode;
        _worldDirName = worldName + "/";
        _generator = std::make_shared<ProceduralWorldGenerator>();
    }

    void loadPosition(glm::vec3 position);

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
        // _dynamicWorld->loadPosition(cameraNode->transform().position());
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

    worldNode = std::make_shared<GLS::Node>();
    scene.rootNode()->addChildNode(worldNode);
    _dynamicWorld = std::make_shared<DynamicWorld>(worldNode, "world");

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

    {
        for (int wx = 0; wx < 30; wx++) {
            for (int wz = 0; wz < 30; wz++) {
                std::shared_ptr<GLS::Node> chunkNode = std::make_shared<GLS::Node>();
                std::shared_ptr<GLS::VoxelChunk> chunk = std::make_shared<GLS::VoxelChunk>();
                chunk->setMaterial(texturedMaterial);
                glm::vec3 chunkPos = glm::vec3(wx * CHUNKSIZE, 0, wz * CHUNKSIZE);
                chunkNode->transform().setPosition(chunkPos);
                for (int x = 0; x < CHUNKSIZE; x++)
                    for (int z = 0; z < CHUNKSIZE; z++) {
                        double nValue = smoothNoise((wx * CHUNKSIZE + x) / 13.37,
                                                    0.42,
                                                    (wz * CHUNKSIZE + z) / 13.37);
                        int groundHeight = nValue * 15 + 7;
                        if (groundHeight > CHUNKSIZE)
                            groundHeight = CHUNKSIZE;
                        for (int y = 0; y < groundHeight; y++) {
                            chunk->setBlockIdAt(x, y, z, y == groundHeight - 1 ? BLOCK_GRASS : BLOCK_DIRT);
                        }
                    }
                chunk->calculBlockAdjacence();
                chunkNode->addRenderable(GLS::Mesh::voxelChunk(chunk));
                worldNode->addChildNode(chunkNode);
            }
        }
    }

}
