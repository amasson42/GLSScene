
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

GrosSceneController::GrosSceneController(std::shared_ptr<GLSWindow> window) :
ISceneController(window) {
}

GrosSceneController::~GrosSceneController() {
    
}

void GrosSceneController::update() {
    ISceneController::update();

	static float time = 0;
	float deltaTime = _window.lock()->deltaTime();

	time += deltaTime;

	if (time > 1) {
		time = 0;
		// std::cout << "FPS: " << 1 / _window.lock()->deltaTime() << std::endl;
		_window.lock()->setTitle("ft_fox | FPS: " + std::to_string(static_cast<int>(1 / deltaTime)));
	}
	

}

void GrosSceneController::makeScene() {

    AppEnv *env = _window.lock()->getAppEnvPtr();

	std::shared_ptr<GLS::Node> cameraNode = std::make_shared<GLS::Node>();
	std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
	camera->aspect = (_scene->getAspect());
	camera->farZ = (300.0);
	camera->fov = (80.0) * M_PI / 180;
	camera->aspect = (_scene->getAspect());
	cameraNode->setCamera(camera);
	cameraNode->transform().moveBy(CHUNKSIZE / 2, CHUNKSIZE / 2, 25);
	cameraNode->setCamera(camera);

	auto texturedMaterial = std::make_shared<GLS::Material>();
    texturedMaterial->specular = glm::vec3(0.1);
    texturedMaterial->shininess = 64;
    try {
        std::shared_ptr<std::string> textureName = env->getArgument("-texture");
		#ifdef _WIN32 // Just because of the build folder and cmake
        std::string filePath = textureName != nullptr ? *textureName : "../assets/textures/ft_vox_textures.png";
		#else
        std::string filePath = textureName != nullptr ? *textureName : "../assets/textures/ft_vox_textures.png";
        // std::string filePath = textureName != nullptr ? *textureName : "assets/textures/ft_vox_textures.png";
		#endif
        texturedMaterial->texture_diffuse = std::make_shared<GLS::Texture>(filePath);
        texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        texturedMaterial->texture_mask = texturedMaterial->texture_diffuse;
        texturedMaterial->shininess = 0;
    } catch (std::exception& e) {
        std::cout << "error " << e.what() << std::endl;
    }


	// TESTS

	int seed = 0;
	std::shared_ptr<std::string> seedString = env->getArgument("-seed");
	if (seedString) {
		seed = std::stoi(*seedString);
	} else {
		seed = time(NULL);
	}

	std::cout << "Using seed: " << seed << std::endl;

	// srand(seed);

	initNoise(seed);
	int blockType = (rand() % 5) + 1;

	// std::cout << "NOISE " << noise(1 + 0.5, 1 + 0.5, 1 + 0.5) << std::endl;
	// std::cout << "NOISE " << noise(100 + 0.5, 100 + 0.5, 100 + 0.5) << std::endl;

	std::shared_ptr<GLS::VoxelChunk> chunk = std::make_shared<GLS::VoxelChunk>();
	chunk->setMaterial(texturedMaterial);


	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int z = 0; z < CHUNKSIZE; z++) {
			// for (int y = 0; y < CHUNKSIZE; y++) {
				// std::cout << "set block at " << x << "  " << y <<  "  " << z << std::endl; 
			double height = noise(x + 1.5, 1.5, z + 1.5) + 0.75;
			// std::cout << "height: " << height / 2 << std::endl;
			chunk->setBlockIdAt(x, 0, z, blockType);
			if (height > 0.1) {
				chunk->setBlockIdAt(x, 1, z, BLOCK_GRASS);
			}
			// }
		}
	}




	chunk->calculBlockAdjacence();

    std::shared_ptr<GLS::Node> worldNode = std::make_shared<GLS::Node>();
	std::shared_ptr<GLS::Node> chunkNode = std::make_shared<GLS::Node>();

	chunkNode->addRenderable(GLS::Mesh::voxelChunk(chunk));
	chunkNode->transform().setPosition(glm::vec3(0));


	auto ambiantlightNode = std::make_shared<GLS::Node>();
    auto ambiantlight = std::make_shared<GLS::Light>();
    ambiantlight->type = GLS::light_ambiant;
    ambiantlight->color = glm::vec3(1);
    ambiantlightNode->setLight(ambiantlight);
    _scene->rootNode()->addChildNode(ambiantlightNode);


	worldNode->addChildNode(chunkNode);

    _scene->setCameraNode(cameraNode);
    _scene->rootNode()->addChildNode(cameraNode);
    _scene->rootNode()->addChildNode(worldNode);

}

