
#include "AppEnv.hpp"

VoxelProceduralSceneController::VoxelProceduralSceneController(std::shared_ptr<GLSWindow> window) :
ISceneController(window) {
	lt = 0;
	cameraMoveSpeed = 5;
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

	if (et - lt >= 0.05) {
		lt = et;
		_dynamicWorld->loadPosition(cameraNode);
	}
}

void VoxelProceduralSceneController::keyCallBack(int key, int scancode, int action, int mods) {
	ISceneController::keyCallBack(key, scancode, action, mods);

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_EQUAL)
			cameraMoveSpeed *= 2.0;
		if (key == GLFW_KEY_MINUS)
			cameraMoveSpeed /= 2.0;
	}
}

void VoxelProceduralSceneController::makeScene() {

	GLS::Scene& scene(*_scene);
	if (_window.expired())
		return;
	AppEnv *env = _window.lock()->getAppEnvPtr();

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

	std::shared_ptr<GLS::Material> axe_x_material = std::make_shared<GLS::Material>();
	axe_x_material->diffuse = glm::vec3(1, 0, 0);
	std::shared_ptr<GLS::Material> axe_y_material = std::make_shared<GLS::Material>();
	axe_y_material->diffuse = glm::vec3(0, 1, 0);
	std::shared_ptr<GLS::Material> axe_z_material = std::make_shared<GLS::Material>();
	axe_z_material->diffuse = glm::vec3(0, 0, 1);

	std::shared_ptr<GLS::Mesh> axe_x_mesh = GLS::Mesh::cube(160, 1, 1);
	axe_x_mesh->setMaterial(axe_x_material);
	std::shared_ptr<GLS::Mesh> axe_y_mesh = GLS::Mesh::cube(1, 16, 1);
	axe_y_mesh->setMaterial(axe_y_material);
	std::shared_ptr<GLS::Mesh> axe_z_mesh = GLS::Mesh::cube(1, 1, 16);
	axe_z_mesh->setMaterial(axe_z_material);

	std::shared_ptr<GLS::Node> axes_node = std::make_shared<GLS::Node>();
	axes_node->transform().position().y = 60;
	scene.rootNode()->addChildNode(axes_node);

	std::shared_ptr<GLS::Node> axe_x_node = std::make_shared<GLS::Node>();
	axe_x_node->addRenderable(axe_x_mesh);
	axe_x_node->transform().position().x = 8;
	axes_node->addChildNode(axe_x_node);

	std::shared_ptr<GLS::Node> axe_y_node = std::make_shared<GLS::Node>();
	axe_y_node->addRenderable(axe_y_mesh);
	axe_y_node->transform().position().y = 8;
	axes_node->addChildNode(axe_y_node);

	std::shared_ptr<GLS::Node> axe_z_node = std::make_shared<GLS::Node>();
	axe_z_node->addRenderable(axe_z_mesh);
	axe_z_node->transform().position().z = 8;
	axes_node->addChildNode(axe_z_node);

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
