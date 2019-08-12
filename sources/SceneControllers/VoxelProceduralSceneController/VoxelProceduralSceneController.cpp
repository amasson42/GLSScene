
#include "AppEnv.hpp"

#define BLOCK_AIR 0
#define BLOCK_BEDROCK 1
#define BLOCK_STONE 2
#define BLOCK_DIRT 3
#define BLOCK_GRASS 4
#define BLOCK_SAND 5
#define BLOCK_GRAVEL 6
#define BLOCK_WATER 7
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

VoxelProceduralSceneController::VoxelProceduralSceneController(std::shared_ptr<GLSWindow> window):
ISceneController(window) {
	lt = 0;
	cameraMoveSpeed = 5;
	_nanoguiScreen = _window.lock()->nanoguiScreen();
	_pickedBlock = 1;
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

		_lightNode->transform().setPosition(cameraNode->transform().position());

		// Update the ui if needed
		if (_displayInterface) {
			_fpsValueLabel->setCaption(std::to_string(static_cast<int>(1.0f / _window.lock()->deltaTime())));
			glm::vec3 playerPosition = cameraNode->transform().position();
			_playerPositionLabel[0]->setValue(static_cast<int>(playerPosition.x));
			_playerPositionLabel[1]->setValue(static_cast<int>(playerPosition.y));
			_playerPositionLabel[2]->setValue(static_cast<int>(playerPosition.z));

			_speedValueField->setValue(cameraMoveSpeed);

			_nanoguiScreen->performLayout();

			float offsetX = static_cast<float>(3 * (_pickedBlock % 10 - 1) + 1);
			float offsetY = static_cast<float>(_pickedBlock / 10);
			float imgViewOffsetX = _pickedBlockTexture->sizeF().x() * offsetX;
			float imgViewOffsetY = _pickedBlockTexture->sizeF().y() * offsetY;
			_pickedBlockTexture->setOffset(nanogui::Vector2f(-imgViewOffsetX, -imgViewOffsetY));
		}
	}

	{
		glm::vec3 axesOffset = cameraNode->transform().matrix() * glm::vec4(0, 0, -2, 1);
		_axesNode->transform().setPosition(axesOffset);
		glm::vec3 placePositionOfDoom = glm::vec3(cameraNode->transform().matrix() * glm::vec4(0, 0, -2, 1));
		placePositionOfDoom.x = std::floor(placePositionOfDoom.x) + 0.5f;
		placePositionOfDoom.y = std::floor(placePositionOfDoom.y) + 0.5f;
		placePositionOfDoom.z = std::floor(placePositionOfDoom.z) + 0.5f;
		_placeHolderBlockOfDoom->transform().setPosition(placePositionOfDoom);
	}
}

void VoxelProceduralSceneController::updateUI() {
	if (_window.expired())
		return ;
	auto window = _window.lock();

	if (_displayInterface) {
		window->setMouseInputMode(GLFW_CURSOR_NORMAL);
		this->setCameraMouseControl(false);
		_axesNode->setActive(true);
		_playerWindow->setVisible(true);
		_environementWindow->setVisible(true);

		nanogui::Vector2i playerWindowNewPos = _playerWindow->position();
		if (_playerWindow->position().x() + _playerWindow->width() > window->size().x)
			playerWindowNewPos.x() = window->size().x - _playerWindow->width();
		if (_playerWindow->position().y() + _playerWindow->height() > window->size().y)
			playerWindowNewPos.y() = window->size().y - _playerWindow->height();
		_playerWindow->setPosition(playerWindowNewPos);

		nanogui::Vector2i environementWindowNewPos = _environementWindow->position();
		if (_environementWindow->position().x() + _environementWindow->width() > window->size().x)
			environementWindowNewPos.x() = window->size().x - _environementWindow->width();
		if (_environementWindow->position().y() + _environementWindow->height() > window->size().y)
			environementWindowNewPos.y() = window->size().y - _environementWindow->height();
		_environementWindow->setPosition(environementWindowNewPos);

	} else {
		window->setMouseInputMode(GLFW_CURSOR_DISABLED);
		this->setCameraMouseControl(true);
		_axesNode->setActive(false);
		_playerWindow->setVisible(false);
		_environementWindow->setVisible(false);
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

	// Open the nanogui interface and display information
	// Also disable the mouse camera control
	if (key == GLFW_KEY_I && action == GLFW_PRESS && !_generatorKernelField->focused()) {
		_displayInterface = !_displayInterface;
		updateUI();
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_TAB) {
		for (int i = 0; i < 2; i++) {
			if (_playerPositionLabel[i]->focused()) {
				_playerPositionLabel[i]->focusEvent(false);
				_playerPositionLabel[i + 1]->requestFocus();
				break;
			}
		}
	}
}

void VoxelProceduralSceneController::scrollCallBack(double x, double y) {
	std::map<int, std::string>::const_iterator it;

	if (y > 0.0) {
		it = VoxelProceduralSceneController::_BlockNames.find(_pickedBlock);
		if (std::next(it) != VoxelProceduralSceneController::_BlockNames.end()) {
			it++;
		} else {
			it = VoxelProceduralSceneController::_BlockNames.begin();
		}
	} else if (y < 0.0) {
		it = VoxelProceduralSceneController::_BlockNames.find(_pickedBlock);
		
		if (it == VoxelProceduralSceneController::_BlockNames.begin()) {
			it = std::prev(VoxelProceduralSceneController::_BlockNames.end());
		} else {
			it--;
		}
	}
	if (y != 0.0) {
		_pickedBlock = it->first;
		_pickedBlockLabel->setCaption(it->second);
		_handBlock->voxel->setBlockIdAt(0, 0, 0, _pickedBlock);
		_handBlock->voxel->calculBlockAdjacence();
		_handBlock->updateMesh();
	}
}

void VoxelProceduralSceneController::mouseButtonCallBack(int button, int action, int modifiers) {
	// std::cout << button << " => " << action << std::endl;
	glm::vec3 targetWorldPos = _placeHolderBlockOfDoom->transform().position();
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		_dynamicWorld->setBlockAt(targetWorldPos, BLOCK_AIR);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		_dynamicWorld->setBlockAt(targetWorldPos, _pickedBlock);
	}
}

void VoxelProceduralSceneController::resizeWindowCallBack(glm::vec2 newSize) {
	ISceneController::resizeWindowCallBack(newSize);
	updateUI();
}

void VoxelProceduralSceneController::makeScene() {

	GLS::Scene& scene(*_scene);
	if (_window.expired())
		return;

	std::shared_ptr<GLSWindow> window = _window.lock();
	AppEnv *env = window->getAppEnvPtr();

	std::string voxelTextureFilePath;
	{
		std::shared_ptr<std::string> textureName = env->getArgument("-texture");
		voxelTextureFilePath = textureName != nullptr ? *textureName : "assets/textures/ft_vox_textures_2.png";
	}
	std::string generatorFilePath;
	{
		std::shared_ptr<std::string> argName = env->getArgument("-generator");
		// generatorFilePath = argName != nullptr ? *argName : "assets/voxelProceduralGeneratorSources/hillsGenerator.cl";
		generatorFilePath = argName != nullptr ? *argName : "assets/voxelProceduralGeneratorSources/canyon.cl";
	}
	std::string worldName;
	{
		std::shared_ptr<std::string> argName = env->getArgument("-world");
		worldName = argName != nullptr ? *argName : "world";
	}

	// Material initialisation
	auto texturedMaterial = std::make_shared<GLS::Material>();
	texturedMaterial->specular = glm::vec3(0.1f);
	texturedMaterial->shininess = 64.0f;
	try {
		texturedMaterial->texture_diffuse = std::make_shared<GLS::Texture>(voxelTextureFilePath);

		// texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		// texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		texturedMaterial->texture_diffuse->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		texturedMaterial->texture_mask = texturedMaterial->texture_diffuse;
		texturedMaterial->shininess = 0;
	} catch (std::exception& e) {
		std::cout << "error " << e.what() << std::endl;
	}

	// World
	worldNode = std::make_shared<GLS::Node>();
	scene.rootNode()->addChildNode(worldNode);
	_dynamicWorld = std::make_shared<DynamicWorld>(worldNode, worldName);
	_dynamicWorld->getGenerator()->usedMaterial = texturedMaterial;
	_dynamicWorld->getGenerator()->setSeed(static_cast<unsigned>(time(NULL)));
	_dynamicWorld->getGenerator()->setGenerationKernel(generatorFilePath);

	// Lights

	_lightNode = std::make_shared<GLS::Node>();

	auto directionLightNode = std::make_shared<GLS::Node>();
	directionLightNode->transform().setEulerAngles(glm::vec3(-M_PI / 5, M_PI / 3, 0));
	_lightNode->addChildNode(directionLightNode);

	auto pointlightNode = std::make_shared<GLS::Node>();
	pointlightNode->transform().setPosition(glm::vec3(0, 0, 120));
	directionLightNode->addChildNode(pointlightNode);

	auto pointlight = std::make_shared<GLS::Light>();
	pointlight->type = GLS::light_directional;
	pointlight->color = glm::vec3(1, 1, 1);
	pointlight->cast_shadow = true;
	pointlight->cast_shadow_clip_far = 300;
	pointlight->cast_shadow_map_size_width = 4000;
	pointlight->cast_shadow_map_size_height = 4000;
	pointlightNode->setLight(pointlight);
	pointlightNode->transform().scaleBy(glm::vec3(10.0, 10.0, 1.0));

	scene.rootNode()->addChildNode(_lightNode);

	auto ambiantlightNode = std::make_shared<GLS::Node>();
	auto ambiantlight = std::make_shared<GLS::Light>();
	ambiantlight->type = GLS::light_ambiant;
	ambiantlight->color = glm::vec3(0.3f);
	ambiantlightNode->setLight(ambiantlight);
	scene.rootNode()->addChildNode(ambiantlightNode);

	// Camera
	cameraNode = std::make_shared<GLS::Node>();
	cameraNode->transform().moveBy(0, 100, 16);
	std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
	camera->farZ = 200.0f;
	camera->fogFar = 200.0f;
	camera->fogNear = 200.0f * 0.9f;
	camera->fov = static_cast<float>((80.0f) * M_PI / 180.0f);
	scene.setBackgroundColor(glm::vec4(115 / 255.0f, 195 / 255.0f, 1, 1));
	camera->aspect = (scene.getAspect());
	cameraNode->setCamera(camera);
	scene.setCameraNode(cameraNode);
	scene.rootNode()->addChildNode(cameraNode);

	// Skybox
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("assets/textures/skybox/minecraft/minecraft_ft.jpg");
	skyboxFaces.push_back("assets/textures/skybox/minecraft/minecraft_bk.jpg");
	skyboxFaces.push_back("assets/textures/skybox/minecraft/minecraft_up.jpg");
	skyboxFaces.push_back("assets/textures/skybox/minecraft/minecraft_dn.jpg");
	skyboxFaces.push_back("assets/textures/skybox/minecraft/minecraft_rt.jpg");
	skyboxFaces.push_back("assets/textures/skybox/minecraft/minecraft_lf.jpg");
	// skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_ft.jpg");
	// skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_bk.jpg");
	// skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_up.jpg");
	// skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_dn.jpg");
	// skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_rt.jpg");
	// skyboxFaces.push_back("assets/textures/skybox/elbrus/elbrus_lf.jpg");
	try {
		std::shared_ptr<GLS::Skybox> skybox = std::make_shared<GLS::Skybox>(skyboxFaces);
		scene.setSkybox(skybox);
	} catch (std::exception& e) {
		std::cout << "can't load skybox textures with exception: " << e.what() << std::endl;
	}

	// Interface creation
	_displayInterface = true;

	std::shared_ptr<GLS::Material> axe_x_material = std::make_shared<GLS::Material>();
	axe_x_material->diffuse = glm::vec3(1, 0, 0);
	std::shared_ptr<GLS::Material> axe_y_material = std::make_shared<GLS::Material>();
	axe_y_material->diffuse = glm::vec3(0, 1, 0);
	std::shared_ptr<GLS::Material> axe_z_material = std::make_shared<GLS::Material>();
	axe_z_material->diffuse = glm::vec3(0, 0, 1);

	std::shared_ptr<GLS::Mesh> axe_x_mesh = GLS::Mesh::cube(1, 0, 0);
	axe_x_mesh->setMaterial(axe_x_material);
	axe_x_mesh->setDrawMode(GL_LINES);
	std::shared_ptr<GLS::Mesh> axe_y_mesh = GLS::Mesh::cube(0, 1, 0);
	axe_y_mesh->setMaterial(axe_y_material);
	axe_y_mesh->setDrawMode(GL_LINES);
	std::shared_ptr<GLS::Mesh> axe_z_mesh = GLS::Mesh::cube(0, 0, 1);
	axe_z_mesh->setMaterial(axe_z_material);
	axe_z_mesh->setDrawMode(GL_LINES);

	std::shared_ptr<GLS::Node> axes_node = std::make_shared<GLS::Node>();
	axes_node->transform().position().y = 60;
	scene.rootNode()->addChildNode(axes_node);

	std::shared_ptr<GLS::Node> axe_x_node = std::make_shared<GLS::Node>();
	axe_x_node->addRenderable(axe_x_mesh);
	axe_x_node->transform().position().x = 0.5;
	axes_node->addChildNode(axe_x_node);

	std::shared_ptr<GLS::Node> axe_y_node = std::make_shared<GLS::Node>();
	axe_y_node->addRenderable(axe_y_mesh);
	axe_y_node->transform().position().y = 0.5;
	axes_node->addChildNode(axe_y_node);

	std::shared_ptr<GLS::Node> axe_z_node = std::make_shared<GLS::Node>();
	axe_z_node->addRenderable(axe_z_mesh);
	axe_z_node->transform().position().z = 0.5;
	axes_node->addChildNode(axe_z_node);

	_axesNode = axes_node;

	// Player right hand block

	_handBlock = std::make_shared<GameVoxelChunk>();
	_handBlock->voxel->setMaterial(texturedMaterial);
	_handBlock->voxel->setBlockIdAt(0, 0, 0, _pickedBlock);
	_handBlock->voxel->calculBlockAdjacence();
	_handBlock->updateMesh();
	_handBlock->node->transform().scaleBy(glm::vec3(0.5));
	_handBlock->node->transform().setPosition(glm::vec3(0.65, -0.8, -1.5));

	cameraNode->addChildNode(_handBlock->node);

	_placeHolderBlockOfDoom = std::make_shared<GLS::Node>();
	std::shared_ptr<GLS::Mesh> placeHolderMesh = GLS::Mesh::cube(1.02f, 1.02f, 1.02f);
	placeHolderMesh->setDrawMode(GL_LINES);
	_placeHolderBlockOfDoom->addRenderable(placeHolderMesh);
	scene.rootNode()->addChildNode(_placeHolderBlockOfDoom);

	// Player Window
	_playerWindow = new nanogui::Window(_nanoguiScreen, "Player");
	_playerWindow->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Vertical));
	_playerWindow->setPosition(nanogui::Vector2i(10, 10));

	// Create a widget, with a layout
	auto fpsCounterWidget = new nanogui::Widget(_playerWindow);
	fpsCounterWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));

	// Add ui elements to this widget
	new nanogui::Label(fpsCounterWidget, "FPS: ");
	_fpsValueLabel = new nanogui::Label(fpsCounterWidget, "-");

	auto playerPositionWidget = new nanogui::Widget(_playerWindow);
	playerPositionWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));

	for (int i = 0; i < 3; i++) {
		_playerPositionLabel[i] = new nanogui::IntBox<int>(playerPositionWidget, 0);
		_playerPositionLabel[i]->setEditable(true);
	}

	// Callbacks for player teleportation (triggered when int box are modified)
	_playerPositionLabel[0]->setCallback([this](int posX) {
		this->cameraNode->transform().position().x = static_cast<float>(posX);
	});
	_playerPositionLabel[1]->setCallback([this](int posY) {
		this->cameraNode->transform().position().y = static_cast<float>(posY);
	});
	_playerPositionLabel[2]->setCallback([this](int posZ) {
		this->cameraNode->transform().position().z = static_cast<float>(posZ);
	});

	// Speed
	auto speedBox = new nanogui::Widget(_playerWindow);
	speedBox->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	new nanogui::Label(speedBox, "Speed: ");
	_speedValueField = new nanogui::FloatBox<float>(speedBox, cameraMoveSpeed);
	_speedValueField->setEditable(true);
	_speedValueField->setCallback([this](float speed) {
		this->cameraMoveSpeed = speed;
	});

	// FOV
	const float minFov = static_cast<float>(M_PI * 0.16666f);
	const float maxFov = static_cast<float>(M_PI * 0.83333f);
	auto fovBox = new nanogui::Widget(_playerWindow);
	fovBox->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	new nanogui::Label(fovBox, "Fov: ");
	auto fovSlider = new nanogui::Slider(fovBox);
	fovSlider->setValue((camera->fov - minFov) / (maxFov - minFov));
	auto fovValue = new nanogui::FloatBox<float>(fovBox, static_cast<float>((camera->fov * 180.0f) / M_PI));
	fovSlider->setCallback([this, camera, fovValue, minFov, maxFov](float value) {
		camera->fov = value * (maxFov - minFov) + minFov;
		fovValue->setValue(static_cast<float>((camera->fov * 180.0f) / M_PI));
	});

	// Picked Block
	auto pickedBlockBox = new nanogui::Widget(_playerWindow);
	pickedBlockBox->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Vertical));
	
	const int blockImageSize = 50;
	_pickedBlockTexture = new nanogui::ImageView(pickedBlockBox, _dynamicWorld->getGenerator()->usedMaterial->texture_diffuse->buffer());
	_pickedBlockTexture->setFixedSize(nanogui::Vector2i(blockImageSize, blockImageSize));
	_pickedBlockTexture->setScale((float)blockImageSize / texturedMaterial->texture_diffuse->width() * 30.0f); 
	_pickedBlockTexture->setFixedScale(true);
	_pickedBlockTexture->setFixedOffset(false);
	_pickedBlockLabel = new nanogui::Label(pickedBlockBox, VoxelProceduralSceneController::_BlockNames.at(_pickedBlock));

	// Environement GUI

	_environementWindow = new nanogui::Window(_nanoguiScreen, "Environement");
	_environementWindow->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Vertical));

		// render distance
	auto renderDistanceWidget = new nanogui::Widget(_environementWindow);
	renderDistanceWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	new nanogui::Label(renderDistanceWidget, "Render distance: ");
	auto renderDistanceSlider = new nanogui::Slider(renderDistanceWidget);
	auto renderDistanceValue = new nanogui::IntBox<int>(renderDistanceWidget, static_cast<int>(_dynamicWorld->getRenderDistance()));
	renderDistanceSlider->setCallback([this, renderDistanceValue, camera](float value) {
		float renderDistance = value * (DynamicWorld::maxRenderDistance - DynamicWorld::minRenderDistance)
								+ DynamicWorld::minRenderDistance;
		renderDistanceValue->setValue(static_cast<int>(renderDistance));
		_dynamicWorld->setRenderDistance(renderDistance);
		camera->farZ = renderDistance;
		camera->fogFar = renderDistance;
		camera->fogNear = renderDistance * 0.9f;
	});
	renderDistanceSlider->setValue((_dynamicWorld->getRenderDistance() - DynamicWorld::minRenderDistance)
									/ (DynamicWorld::maxRenderDistance - DynamicWorld::minRenderDistance));

		// meshmerize effect
	auto meshmerizerWidget = new nanogui::Widget(_environementWindow);
	meshmerizerWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	new nanogui::Label(meshmerizerWidget, "Meshmerize effect: ");
	auto meshmerizerSlider = new nanogui::Slider(meshmerizerWidget);
	auto meshmerizerValue = new nanogui::FloatBox<float>(meshmerizerWidget, GameVoxelChunk::meshmerizerIntensity);
	meshmerizerSlider->setCallback([this, meshmerizerValue](float value) {
		GameVoxelChunk::meshmerizerIntensity = value;
		meshmerizerValue->setValue(GameVoxelChunk::meshmerizerIntensity);
	});
	renderDistanceSlider->setValue((_dynamicWorld->getRenderDistance() - DynamicWorld::minRenderDistance)
									/ (DynamicWorld::maxRenderDistance - DynamicWorld::minRenderDistance));

		// useless button
	auto uselessButtonWidget = new nanogui::Widget(_environementWindow);
	uselessButtonWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	auto uselessButton = new nanogui::Button(uselessButtonWidget, "Boutton inutile");
	uselessButton->setCallback([]() {
		std::cout << "Je ne sert a rien" << std::endl;
	});

		// seed field
	auto seedWidget = new nanogui::Widget(_environementWindow);
	seedWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	new nanogui::Label(seedWidget, "seed: ");
	auto seedField = new nanogui::IntBox<unsigned int>(seedWidget, _dynamicWorld->getGenerator()->getSeed());
	seedField->setEditable(true);
	seedField->setCallback([this](unsigned int value) {
		_dynamicWorld->getGenerator()->setSeed(value);
	});

		// generator kernel choosing
	auto generatorKernelWidget = new nanogui::Widget(_environementWindow);
	generatorKernelWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Vertical));
	_generatorKernelField = new nanogui::TextBox(generatorKernelWidget, generatorFilePath);
	_generatorKernelField->setEditable(true);

		// reload button
	auto reloadWidget = new nanogui::Widget(_environementWindow);
	reloadWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	(new nanogui::Button(reloadWidget, "Reload"))
		->setCallback([this]() {
		try {
			_dynamicWorld->getGenerator()->setGenerationKernel(_generatorKernelField->value());
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
		_dynamicWorld->reloadChunks();
	});

	updateUI();

	// Compute the layout (width, height)
	_nanoguiScreen->performLayout();

	_environementWindow->setPosition(nanogui::Vector2i(window->size().x - _environementWindow->width() - 10, 10));

}

const std::map<int, std::string> VoxelProceduralSceneController::_BlockNames = {
	{ BLOCK_BEDROCK, "Bedrock" },
	{ BLOCK_STONE, "Stone" },
	{ BLOCK_DIRT, "Dirt" },
	{ BLOCK_GRASS, "Grass" },
	{ BLOCK_SAND, "Sand" },
	{ BLOCK_GRAVEL, "Gravel" },
	{ BLOCK_WATER, "Water" },
	{ BLOCK_GRASS_BROWN, "Brown Grass" },
	{ BLOCK_WOOD, "Wood" },
	{ BLOCK_LEAFS, "Leaf" },
	{ BLOCK_WOOD_PLANKS, "Oak Plank" },
	{ BLOCK_BRICKS, "Brick" },
	{ BLOCK_COBBLESTONE, "Cobblestone" },
	{ BLOCK_ICE, "Ice" },
	{ BLOCK_ICE_BROKEN, "Broken Ice" },
	{ BLOCK_OBSIDIAN, "Obsidian" },
	{ BLOCK_GRASS_PURPLE, "Purple Grass" },
	{ BLOCK_GOLD, "Gold" },
	{ BLOCK_TNT, "TNT" },
};