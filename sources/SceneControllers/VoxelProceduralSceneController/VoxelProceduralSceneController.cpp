
#include "AppEnv.hpp"
#include <dirent.h>

VoxelProceduralSceneController::VoxelProceduralSceneController(std::shared_ptr<GLSWindow> window):
ISceneController(window) {
	lt = 0;
	cameraMoveSpeed = 5;
	_nanoguiScreen = _window.lock()->nanoguiScreen();
	_startupWindow = DisplayedWindow::StartMenu;
	_pickedBlockIndex = 0;
	_selectWorldWindow = nullptr;
	_newWorldWindow = nullptr;
	_cinematicMode = false;

	_createWorldsFolder();
}

VoxelProceduralSceneController::~VoxelProceduralSceneController() {

}

void VoxelProceduralSceneController::update() {
	if (_startupWindow != DisplayedWindow::Game) {
		return;
	}

	ISceneController::update();
	if (_window.expired())
		return;
	float et = _window.lock()->elapsedTime();

	if (et - lt >= 0.05) {
		lt = et;

		if (mustUpdate)
			_dynamicWorld->loadPosition(cameraNode);

		_lightNode->transform().setPosition(cameraNode->transform().position());

		// Update the ui if needed
		if (_displayInterface) {
			_fpsValueLabel->set_caption(std::to_string(static_cast<int>(1.0f / _window.lock()->deltaTime())));
			glm::vec3 playerPosition = cameraNode->transform().position();
			_playerPositionLabel[0]->set_value(static_cast<int>(playerPosition.x));
			_playerPositionLabel[1]->set_value(static_cast<int>(playerPosition.y));
			_playerPositionLabel[2]->set_value(static_cast<int>(playerPosition.z));

			_speedValueField->set_value(cameraMoveSpeed);

			_nanoguiScreen->perform_layout();

			int pickedBlockId = _pickableBlocks[_pickedBlockIndex].second.textureId;
			float offsetX = static_cast<float>(3 * (pickedBlockId % 16));
			float offsetY = static_cast<float>(2 * (pickedBlockId / 16));
			float imgViewOffsetX = _pickedBlockTexture->width() * offsetX;
			float imgViewOffsetY = _pickedBlockTexture->height() * offsetY;
			_pickedBlockTexture->set_offset(nanogui::Vector2f(-imgViewOffsetX, -imgViewOffsetY));
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
		_playerWindow->set_visible(true);
		_environementWindow->set_visible(true);

		nanogui::Vector2i playerWindowNewPos = _playerWindow->position();
		if (_playerWindow->position().x() + _playerWindow->width() > window->size().x)
			playerWindowNewPos.x() = window->size().x - _playerWindow->width();
		if (_playerWindow->position().y() + _playerWindow->height() > window->size().y)
			playerWindowNewPos.y() = window->size().y - _playerWindow->height();
		_playerWindow->set_position(playerWindowNewPos);

		nanogui::Vector2i environementWindowNewPos = _environementWindow->position();
		if (_environementWindow->position().x() + _environementWindow->width() > window->size().x)
			environementWindowNewPos.x() = window->size().x - _environementWindow->width();
		if (_environementWindow->position().y() + _environementWindow->height() > window->size().y)
			environementWindowNewPos.y() = window->size().y - _environementWindow->height();
		_environementWindow->set_position(environementWindowNewPos);

	} else {
		window->setMouseInputMode(GLFW_CURSOR_DISABLED);
		this->setCameraMouseControl(true);
		_axesNode->setActive(false);
		_playerWindow->set_visible(false);
		_environementWindow->set_visible(false);
	}
}

void VoxelProceduralSceneController::keyCallBack(int key, int scancode, int action, int mods) {
	ISceneController::keyCallBack(key, scancode, action, mods);
	if (_startupWindow != DisplayedWindow::Game) {
		return;
	}
	if (_playerWindow->focused() || _environementWindow->focused()) {
		return ;
	}
	if (action == GLFW_PRESS) {
		if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
			_pickableBlocks[_pickedBlockIndex].second.meshType = GLS::VoxelBlockMeshType(key - GLFW_KEY_0);
			_handBlock->voxel->blockAt(glm::ivec3(0, 0, 0)) = _pickableBlocks[_pickedBlockIndex].second;
			_updateHandBlock();
		}
		if (key == GLFW_KEY_EQUAL)
			cameraMoveSpeed *= 2.0;
		if (key == GLFW_KEY_MINUS)
			cameraMoveSpeed /= 2.0;
		cameraMoveSpeed = std::clamp(cameraMoveSpeed, 0.0f, 1000.0f);
		if (key == GLFW_KEY_C) {
			_toggleCinematicMode();
		}
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
				_playerPositionLabel[i]->focus_event(false);
				_playerPositionLabel[i + 1]->request_focus();
				break;
			}
		}
	}
}

void VoxelProceduralSceneController::scrollCallBack(double x, double y) {
	if (_startupWindow != DisplayedWindow::Game) {
		return;
	}
	if (y > 0.0) {
		_pickedBlockIndex = (++_pickedBlockIndex) % _pickableBlocks.size();
	} else if (y < 0.0) {
		_pickedBlockIndex = (--_pickedBlockIndex) < 0 ? _pickableBlocks.size() - 1 : _pickedBlockIndex;
	}
	if (y != 0.0) {
		_pickedBlockLabel->set_caption(_pickableBlocks[_pickedBlockIndex].first);
		_handBlock->voxel->blockAt(glm::ivec3(0, 0, 0)) = _pickableBlocks[_pickedBlockIndex].second;
		_updateHandBlock();
		_handBlock->mesh->generateBuffers();
	}
}

void VoxelProceduralSceneController::mouseButtonCallBack(int button, int action, int modifiers) {
	if (_startupWindow != DisplayedWindow::Game) {
		return;
	}
	glm::vec3 targetWorldPos = _placeHolderBlockOfDoom->transform().position();
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		_dynamicWorld->setBlockAt(targetWorldPos, GLS::VoxelBlock(GLS::VoxelBlockMeshType::Empty, 0));
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		_dynamicWorld->setBlockAt(targetWorldPos, _pickableBlocks[_pickedBlockIndex].second);
	}
}

void VoxelProceduralSceneController::closeCallback() {
	ISceneController::closeCallback();
	if (_startupWindow != DisplayedWindow::Game) {
		return;
	}
	_saveJsonFileInfo();
	_dynamicWorld->saveLoadedChunks();

}

void VoxelProceduralSceneController::resizeWindowCallBack(glm::vec2 newSize) {
	if (_startupWindow != DisplayedWindow::Game) {
		return;
	}
	ISceneController::resizeWindowCallBack(newSize);
	updateUI();
}

void VoxelProceduralSceneController::makeScene() {

	if (_startupWindow == DisplayedWindow::StartMenu) {
		// Saved worlds scrolling list
		if (_selectWorldWindow) {
			_selectWorldWindow->set_visible(true);
			return ;
		}
		_selectWorldWindow = new nanogui::Window(_nanoguiScreen, "Saved worlds");
		_selectWorldWindow->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Fill, 0, 0));

		nanogui::VScrollPanel* scrollPanel = new nanogui::VScrollPanel(_selectWorldWindow);
		scrollPanel->set_fixed_size(nanogui::Vector2i(200, 300));

		nanogui::Widget* worldsWidget = new nanogui::Widget(scrollPanel);
		worldsWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Minimum, 10, 0));

		DIR* dir;
		struct dirent* ent;
		if ((dir = opendir("worlds")) != nullptr) {
			while ((ent = readdir(dir)) != nullptr) {
				if (ent->d_type == DT_DIR) {
					std::string worldName;
					nlohmann::json jsonData = _readJsonFileInfo((std::string("worlds/" + std::string(ent->d_name) + "/info.json")).c_str());
					try {
						worldName = jsonData.at("worldName").get<std::string>();
					} catch (const nlohmann::json::exception& e) {
						continue ;
					}
		 			nanogui::Button* l = new nanogui::Button(worldsWidget, (worldName.length() < 24 ? worldName : worldName.substr(0, 22) + "..."));
					l->set_fixed_width(180);
		 			l->set_callback([worldName, this]() {
		 				_startupWindow = DisplayedWindow::Game;
		 				_setupWorld();
		 				_setupGUI();
						try {
							_loadJsonFileInfo(_readJsonFileInfo(VoxelProceduralSceneController::worldDirPrefix + worldName + "/info.json"));
						} catch (const std::exception& e) {
							throw std::runtime_error("world was deleted while being use by a dumbfuck corrector... seriously fuck that guy");
						}
		 				_selectWorldWindow->set_visible(false);
		 			});
				}
			}
			closedir(dir);
		}

		// New world Button
		nanogui::Widget* newWorldWidget = new nanogui::Widget(_selectWorldWindow);
		newWorldWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Maximum, 10, 0));
		nanogui::Button* newWorldButton = new nanogui::Button(newWorldWidget, "New World");
		
		newWorldButton->set_callback([this]() {
			_startupWindow = DisplayedWindow::NewWorld;
			_selectWorldWindow->set_visible(false);
			makeScene();
		});

		_nanoguiScreen->perform_layout();
		_selectWorldWindow->center();
		return ;
	} else if (_startupWindow == DisplayedWindow::NewWorld) {
		// New World window
		if (_newWorldWindow) {
			_newWorldWindow->set_visible(true);
			return;
		}
		_newWorldWindow = new nanogui::Window(_nanoguiScreen, "New World");
		_newWorldWindow->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Middle, 10, 10));

		// World Name
		auto worldNameWidget = new nanogui::Widget(_newWorldWindow);
		worldNameWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Vertical));
		
		auto worldNameLabel = new nanogui::Label(worldNameWidget, "World Name");

		auto worldNameField = new nanogui::TextBox(worldNameWidget, "");
		worldNameField->set_editable(true);
		worldNameField->set_fixed_width(200);

		// World Seed
		auto worldSeedWidget = new nanogui::Widget(_newWorldWindow);
		worldSeedWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Vertical));

		auto worldSeedLabel = new nanogui::Label(worldNameWidget, "Seed");

		auto worldSeedField = new nanogui::IntBox<int>(worldNameWidget);
		std::srand(std::time(nullptr));
		worldSeedField->set_value(std::rand());
		worldSeedField->set_editable(false);
		worldSeedField->set_fixed_width(200);

		// World Kernel Generator
		auto worldGeneratorWidget = new nanogui::Widget(_newWorldWindow);
		worldGeneratorWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Vertical));

		auto worldGeneratorLabel = new nanogui::Label(worldNameWidget, "Generator");

		auto worldGeneratorField = new nanogui::TextBox(worldNameWidget, _generatorFileName);
		worldGeneratorField->set_editable(true);
		worldGeneratorField->set_fixed_width(200);

		// Apply Buttons
		nanogui::Widget* newWorldWidget = new nanogui::Widget(_newWorldWindow);
		newWorldWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 10));
		//		Back Button
		nanogui::Button* backButton = new nanogui::Button(newWorldWidget, "Back");
		backButton->set_callback([this]() {
			_startupWindow = DisplayedWindow::StartMenu;
			_newWorldWindow->set_visible(false);
			makeScene();
		});

		//		New World Button
		nanogui::Button* newWorldButton = new nanogui::Button(newWorldWidget, "New World");
		newWorldButton->set_callback([this, worldNameField, worldSeedField, worldGeneratorField]() {
			if (worldGeneratorField->value().empty() || worldNameField->value().empty()) {
				return;
			}
			std::string worldName = worldNameField->value();
			DIR* dir;
			struct dirent* ent;
			if ((dir = opendir("worlds")) != nullptr) {
				bool canContinue = false;
				while (!canContinue) {
					bool found = false;
					while ((ent = readdir(dir)) != nullptr) {
						if (("world_" + worldName).compare(ent->d_name) == 0) {
							worldName += "_bis";
							found = true;
						}
					}
					if (!found) {
						canContinue = true;
					}
				}
			}
			if (!_dynamicWorld) {
				_setupWorld();
			}
			try {
				_generatorFileName = worldGeneratorField->value();
				_dynamicWorld->getGenerator()->setGenerationKernel(generatorFilePath + worldGeneratorField->value());
				_startupWindow = DisplayedWindow::Game;
				_newWorldWindow->set_visible(false);
				_dynamicWorld->getGenerator()->setSeed(worldSeedField->value());
				_dynamicWorld->setWorldName(worldName);
				_setupGUI();

				_updateWorldFolder();
			} catch (const std::runtime_error& e) {
				std::cerr << e.what() << std::endl;
				return ;
			}
		});
		_nanoguiScreen->perform_layout();
		_newWorldWindow->center();
	}
}

void VoxelProceduralSceneController::_setupWorld() {

	if (_window.expired())
		return;

	std::shared_ptr<GLSWindow> window = _window.lock();
	AppEnv* env = window->getAppEnvPtr();


	std::string voxelTextureFilePath;
	{
		std::shared_ptr<std::string> textureName = env->getArgument("-texture");
		voxelTextureFilePath = textureName != nullptr ? *textureName : "assets/textures/ft_vox_textures_2.png";
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
	_scene->rootNode()->addChildNode(worldNode);
	_dynamicWorld = std::make_shared<DynamicWorld>(worldNode, _scene);
	_dynamicWorld->getGenerator()->usedMaterial = texturedMaterial;

	_setupLights();
	_setupCamera();

	// Skybox
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("assets/textures/skybox/minecraft/minecraft_ft.jpg");
	skyboxFaces.push_back("assets/textures/skybox/minecraft/minecraft_bk.jpg");
	skyboxFaces.push_back("assets/textures/skybox/minecraft/minecraft_up.jpg");
	skyboxFaces.push_back("assets/textures/skybox/minecraft/minecraft_dn.jpg");
	skyboxFaces.push_back("assets/textures/skybox/minecraft/minecraft_rt.jpg");
	skyboxFaces.push_back("assets/textures/skybox/minecraft/minecraft_lf.jpg");
	try {
		std::shared_ptr<GLS::Skybox> skybox = std::make_shared<GLS::Skybox>(skyboxFaces);
		_scene->setSkybox(skybox);
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
	axe_x_mesh->setCastShadowFace(GL_NONE);
	std::shared_ptr<GLS::Mesh> axe_y_mesh = GLS::Mesh::cube(0, 1, 0);
	axe_y_mesh->setMaterial(axe_y_material);
	axe_y_mesh->setDrawMode(GL_LINES);
	axe_y_mesh->setCastShadowFace(GL_NONE);
	std::shared_ptr<GLS::Mesh> axe_z_mesh = GLS::Mesh::cube(0, 0, 1);
	axe_z_mesh->setMaterial(axe_z_material);
	axe_z_mesh->setDrawMode(GL_LINES);
	axe_z_mesh->setCastShadowFace(GL_NONE);

	std::shared_ptr<GLS::Node> axes_node = std::make_shared<GLS::Node>();
	axes_node->transform().position().y = 60;
	_scene->rootNode()->addChildNode(axes_node);

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
	_handBlock->voxel->blockAt(glm::ivec3(0, 0, 0)) = _pickableBlocks[_pickedBlockIndex].second;
	_updateHandBlock();
	_handBlock->node->transform().scaleBy(glm::vec3(0.5));
	_handBlock->node->transform().setPosition(glm::vec3(0.65, -0.8, -1.5));

	cameraNode->addChildNode(_handBlock->node);

	_placeHolderBlockOfDoom = std::make_shared<GLS::Node>();
	std::shared_ptr<GLS::Mesh> placeHolderMesh = GLS::Mesh::cube(1.02f, 1.02f, 1.02f);
	placeHolderMesh->setDrawMode(GL_LINES);
	placeHolderMesh->setCastShadowFace(GL_NONE);
	_placeHolderBlockOfDoom->addRenderable(placeHolderMesh);
	_scene->rootNode()->addChildNode(_placeHolderBlockOfDoom);
}

void VoxelProceduralSceneController::_setupLights() {
	_lightNode = std::make_shared<GLS::Node>();

	auto directionLightRotation = std::make_shared<GLS::Node>();
	directionLightRotation->transform().setEulerAngles(glm::vec3(-M_PI / 5, M_PI / 3, 0));
	_lightNode->addChildNode(directionLightRotation);

	_directionLightNode = std::make_shared<GLS::Node>();
	_directionLightNode->transform().setPosition(glm::vec3(0, 0, 120));
	directionLightRotation->addChildNode(_directionLightNode);

	auto directionLight = std::make_shared<GLS::Light>();
	directionLight->type = GLS::light_directional;
	directionLight->color = glm::vec3(1, 1, 1);
	directionLight->cast_shadow = false;
	directionLight->cast_shadow_clip_far = 300;
	directionLight->cast_shadow_map_size_width = 4000;
	directionLight->cast_shadow_map_size_height = 4000;
	_directionLightNode->setLight(directionLight);
	_directionLightNode->transform().scaleBy(glm::vec3(10.0, 10.0, 1.0));

	auto pointLightNode = std::make_shared<GLS::Node>();
	_lightNode->addChildNode(pointLightNode);
	auto pointLight = std::make_shared<GLS::Light>();
	pointLight->type = GLS::light_point;
	pointLight->color = glm::vec3(0.1, 0.1, 0.1);
	pointLightNode->setLight(pointLight);

	_scene->rootNode()->addChildNode(_lightNode);

	_ambiantlightNode = std::make_shared<GLS::Node>();
	auto ambiantlight = std::make_shared<GLS::Light>();
	ambiantlight->type = GLS::light_ambiant;
	ambiantlight->color = glm::vec3(0.1f);
	_ambiantlightNode->setLight(ambiantlight);
	_scene->rootNode()->addChildNode(_ambiantlightNode);
}

void VoxelProceduralSceneController::_setupCamera() {
	cameraNode = std::make_shared<GLS::Node>();
	cameraNode->transform().moveBy(0, 100, 16);
	std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
	camera->farZ = 200.0f;
	camera->fogFar = 200.0f;
	camera->fogNear = 200.0f * 0.9f;
	camera->fov = static_cast<float>((80.0f) * M_PI / 180.0f);
	_scene->setBackgroundColor(glm::vec4(115 / 255.0f, 195 / 255.0f, 1, 1));
	camera->aspect = (_scene->getAspect());
	cameraNode->setCamera(camera);
	_scene->setCameraNode(cameraNode);
	_scene->rootNode()->addChildNode(cameraNode);
}

void VoxelProceduralSceneController::_setupGUI() {
	// Player Window
	_playerWindow = new nanogui::Window(_nanoguiScreen, "Player");
	_playerWindow->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Middle, 10, 3));
	_playerWindow->set_position(nanogui::Vector2i(10, 10));

	// Create a widget, with a layout
	auto fpsCounterWidget = new nanogui::Widget(_playerWindow);
	fpsCounterWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));

	// Add ui elements to this widget
	new nanogui::Label(fpsCounterWidget, "FPS: ");
	_fpsValueLabel = new nanogui::Label(fpsCounterWidget, "-");

	auto playerPositionWidget = new nanogui::Widget(_playerWindow);
	playerPositionWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));

	for (int i = 0; i < 3; i++) {
		_playerPositionLabel[i] = new nanogui::IntBox<int>(playerPositionWidget, 0);
		_playerPositionLabel[i]->set_editable(true);
	}

	// Callbacks for player teleportation (triggered when int box are modified)
	_playerPositionLabel[0]->set_callback([this](int posX) {
		this->cameraNode->transform().position().x = static_cast<float>(posX);
	});
	_playerPositionLabel[1]->set_callback([this](int posY) {
		this->cameraNode->transform().position().y = static_cast<float>(posY);
	});
	_playerPositionLabel[2]->set_callback([this](int posZ) {
		this->cameraNode->transform().position().z = static_cast<float>(posZ);
	});

	// Speed
	auto speedBox = new nanogui::Widget(_playerWindow);
	speedBox->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	new nanogui::Label(speedBox, "Speed: ");
	_speedValueField = new nanogui::FloatBox<float>(speedBox, cameraMoveSpeed);
	_speedValueField->set_editable(true);
	_speedValueField->set_callback([this](float speed) {
		speed = std::clamp(speed, 0.0f, 1000.0f);
		this->cameraMoveSpeed = speed;
	});

	// FOV
	const float minFov = static_cast<float>(M_PI * 0.16666f);
	const float maxFov = static_cast<float>(M_PI * 0.83333f);
	auto fovBox = new nanogui::Widget(_playerWindow);
	fovBox->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	new nanogui::Label(fovBox, "Fov: ");
	_fovSlider = new nanogui::Slider(fovBox);
	_fovSlider->set_value((cameraNode->camera()->fov - minFov) / (maxFov - minFov));
	_fovValue = new nanogui::FloatBox<float>(fovBox, static_cast<float>((cameraNode->camera()->fov * 180.0f) / M_PI));
	_fovSlider->set_callback([this, minFov, maxFov](float value) {
		cameraNode->camera()->fov = value * (maxFov - minFov) + minFov;
		_fovValue->set_value(static_cast<float>((cameraNode->camera()->fov * 180.0f) / M_PI));
	});

	// Picked Block
	auto pickedBlockBox = new nanogui::Widget(_playerWindow);
	pickedBlockBox->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Vertical));
	
	const int blockImageSize = 50;
	// _pickedBlockTexture = new nanogui::ImageView(pickedBlockBox, _dynamicWorld->getGenerator()->usedMaterial->texture_diffuse->buffer());
	_pickedBlockTexture = new nanogui::ImageView(pickedBlockBox);
	// _pickedBlockTexture->set_image(_dynamicWorld->getGenerator()->usedMaterial->texture_diffuse->buffer());
	_pickedBlockTexture->set_fixed_size(nanogui::Vector2i(blockImageSize, blockImageSize));
	_pickedBlockTexture->set_scale((float)blockImageSize / _dynamicWorld->getGenerator()->usedMaterial->texture_diffuse->width() * 3 * 16.0f); 
	// _pickedBlockTexture->set_fixed_scale(true);
	// _pickedBlockTexture->set_fixed_offset(false);
	_pickedBlockLabel = new nanogui::Label(pickedBlockBox, _pickableBlocks[_pickedBlockIndex].first);

	// Environement GUI

	_environementWindow = new nanogui::Window(_nanoguiScreen, "Environement");
	_environementWindow->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Middle, 10, 3));

		// render distance
	auto renderDistanceWidget = new nanogui::Widget(_environementWindow);
	renderDistanceWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	auto renderDistanceLabel = new nanogui::Label(renderDistanceWidget, "Render distance: ");
	_renderDistanceSlider = new nanogui::Slider(renderDistanceWidget);
	_renderDistanceValue = new nanogui::FloatBox<float>(renderDistanceWidget, _dynamicWorld->getRenderDistance());
	_renderDistanceValue->number_format("%3.0f");
	_renderDistanceValue->set_fixed_width(75);
	renderDistanceLabel->set_fixed_width(125);
	_renderDistanceSlider->set_callback([this](float value) {
		float renderDistance = value * (DynamicWorld::maxRenderDistance - DynamicWorld::minRenderDistance)
								+ DynamicWorld::minRenderDistance;
		_renderDistanceValue->set_value(renderDistance);
		_dynamicWorld->setRenderDistance(renderDistance);
		cameraNode->camera()->farZ = renderDistance;
		cameraNode->camera()->fogFar = renderDistance;
		cameraNode->camera()->fogNear = renderDistance * 0.9f;
	});
	_renderDistanceSlider->set_value((_dynamicWorld->getRenderDistance() - DynamicWorld::minRenderDistance)
									/ (DynamicWorld::maxRenderDistance - DynamicWorld::minRenderDistance));

		// meshmerize effect
	auto meshmerizerWidget = new nanogui::Widget(_environementWindow);
	meshmerizerWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	auto meshmerizerLabel = new nanogui::Label(meshmerizerWidget, "Meshmerize effect: ");
	meshmerizerLabel->set_fixed_width(125);
	_meshmerizerSlider = new nanogui::Slider(meshmerizerWidget);
	_meshmerizerValue = new nanogui::FloatBox<float>(meshmerizerWidget, GameVoxelChunk::meshmerizerIntensity);
	_meshmerizerValue->number_format("%.4f");
	_meshmerizerValue->set_fixed_width(75);
	_meshmerizerValue->set_value(GameVoxelChunk::meshmerizerIntensity);
	_meshmerizerSlider->set_callback([this](float value) {
		GameVoxelChunk::meshmerizerIntensity = value;
		_meshmerizerValue->set_value(GameVoxelChunk::meshmerizerIntensity);
	});
	_renderDistanceSlider->set_value((_dynamicWorld->getRenderDistance() - DynamicWorld::minRenderDistance)
									/ (DynamicWorld::maxRenderDistance - DynamicWorld::minRenderDistance));

		// cast shadow button
	auto directionalLightWidget = new nanogui::Widget(_environementWindow);
	directionalLightWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	_directionalLightCheckbox = new nanogui::CheckBox(directionalLightWidget, "Cast shadows");
	_directionalLightCheckbox->set_callback([this](bool active) {
		_directionLightNode->light()->cast_shadow = active;
	});

		// useless button
	auto uselessButtonWidget = new nanogui::Widget(_environementWindow);
	uselessButtonWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	auto uselessButton = new nanogui::Button(uselessButtonWidget, "Boutton inutile");
	uselessButton->set_callback([]() {
		std::cout << "Je ne sert a rien" << std::endl;
	});

		// seed field
	auto seedWidget = new nanogui::Widget(_environementWindow);
	seedWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	new nanogui::Label(seedWidget, "seed: ");
	_seedField = new nanogui::IntBox<unsigned int>(seedWidget, _dynamicWorld->getGenerator()->getSeed());
	_seedField->set_fixed_width(125);
	_seedField->set_editable(true);
	_seedField->set_callback([this](unsigned int value) {
	});

		// generator kernel choosing
	auto generatorKernelWidget = new nanogui::Widget(_environementWindow);
	generatorKernelWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Vertical));
	_generatorKernelField = new nanogui::TextBox(generatorKernelWidget, _generatorFileName);
	_generatorKernelField->set_editable(true);
	_generatorKernelField->set_fixed_width(200);

		// reload button
	auto reloadWidget = new nanogui::Widget(_environementWindow);
	reloadWidget->set_layout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	(new nanogui::Button(reloadWidget, "Reload"))
		->set_callback([this]() {
		try {
			_dynamicWorld->getGenerator()->setGenerationKernel(generatorFilePath + _generatorKernelField->value());
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
		_dynamicWorld->reloadChunks();
	});

	updateUI();

	// Compute the layout (width, height)
	_nanoguiScreen->perform_layout();

	_environementWindow->set_position(nanogui::Vector2i(_window.lock()->size().x - _environementWindow->width() - 10, 10));
}

void VoxelProceduralSceneController::_createWorldsFolder() {
#ifdef WIN32
	std::wstring folderName = (L"worlds/");
	std::string worldName = std::string(folderName.begin(), folderName.end());
	if (_wmkdir(folderName.c_str()) == EEXIST) {
		std::cerr << "Worlds folder already exists" << std::endl;
	}
#else
	std::string worldName = "worlds/";
	if (mkdir(worldName.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == EEXIST) {
		std::cerr << "Worlds folder already exists" << std::endl;
	}
#endif
}

void VoxelProceduralSceneController::_updateWorldFolder() {
	std::string worldName = VoxelProceduralSceneController::worldDirPrefix + _dynamicWorld->getWorldName();
#ifdef WIN32
	std::wstring folderName;
	folderName.assign(worldName.begin(), worldName.end());
	if (_wmkdir(folderName.c_str()) == -1) {
		std::cerr << "Loading existing world" << std::endl;
	}
#else
	if (mkdir(worldName.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1) {
		std::cerr << "Loading existing world" << std::endl;
	}
#endif
}

void VoxelProceduralSceneController::_saveJsonFileInfo() {
	nlohmann::json data = {
		{
			"worldName", _dynamicWorld->getWorldName(),
		},
		{
			"settings", {
				{ "seed", _dynamicWorld->getGenerator()->getSeed()},
				{ "generator", _generatorKernelField->value()},
				{ "renderDistance", _dynamicWorld->getRenderDistance()},
				{ "meshmerizeEffect", GameVoxelChunk::meshmerizerIntensity},
				{ "castShadow", _directionLightNode->light()->cast_shadow},
			},
		},
		{
			"player", {
				{ "position", { cameraNode->transform().position().x, cameraNode->transform().position().y, cameraNode->transform().position().z } },
				{ "rotation", { _cameraEulerAngles.x, _cameraEulerAngles.y } },
				{ "speed", cameraMoveSpeed },
				{ "fov", static_cast<int>((cameraNode->camera()->fov * 180.0f) / M_PI) + 1 },
				{ "pickedBlock", _pickedBlockIndex },
			}
		}
	};

	std::ofstream infoFile(VoxelProceduralSceneController::worldDirPrefix + _dynamicWorld->getWorldName() + "/info.json");
	if (!infoFile.is_open()) {
		return;
	}
	infoFile << std::setw(4) << data << std::endl;
	infoFile.close();
}

nlohmann::json VoxelProceduralSceneController::_readJsonFileInfo(std::string fileName) {
	std::ifstream jsonFile(fileName);
	nlohmann::json data;

	if (!jsonFile.is_open()) {
		return data;
	}

	try {
		jsonFile >> data;
	} catch (const nlohmann::json::exception& e) {
		return data;
	}

	return data;
}

void VoxelProceduralSceneController::_loadJsonFileInfo(nlohmann::json data) {

	const float minFov = static_cast<float>(M_PI * 0.16666f);
	const float maxFov = static_cast<float>(M_PI * 0.83333f);

	cameraNode->camera()->fov = std::clamp(static_cast<float>(data.at("player").at("fov").get<float>() * M_PI / 180.0f), minFov, maxFov);
	_pickedBlockIndex = std::clamp(static_cast<int>(data.at("player").at("pickedBlock").get<int>()), 0, static_cast<int>(_pickableBlocks.size() - 1));
	cameraMoveSpeed = std::clamp(static_cast<float>(data.at("player").at("speed").get<float>()), 0.01f, 1000.0f);	
	cameraNode->transform().setPosition(glm::vec3(data.at("player").at("position")[0].get<int>(), data.at("player").at("position")[1].get<int>(), data.at("player").at("position")[2].get<int>()));
	_cameraEulerAngles = glm::vec2(
		data.at("player").at("rotation")[0].get<float>(),
		data.at("player").at("rotation")[1].get<float>()
	);

	_speedValueField->set_value(cameraMoveSpeed);
	_playerPositionLabel[0]->set_value(cameraNode->transform().position().x);
	_playerPositionLabel[1]->set_value(cameraNode->transform().position().x);
	_playerPositionLabel[2]->set_value(cameraNode->transform().position().x);
	_fovSlider->set_value((cameraNode->camera()->fov - minFov) / (maxFov - minFov));
	_fovValue->set_value(cameraNode->camera()->fov * 180.0f / M_PI);
	_pickedBlockLabel->set_caption(_pickableBlocks[_pickedBlockIndex].first);
	_handBlock->voxel->blockAt(glm::ivec3(0, 0, 0)) = _pickableBlocks[_pickedBlockIndex].second;
	_updateHandBlock();

	_dynamicWorld->getGenerator()->setSeed(data.at("settings").at("seed").get<int>());
	_dynamicWorld->getGenerator()->setGenerationKernel(generatorFilePath + data.at("settings").at("generator").get<std::string>());
	_dynamicWorld->setRenderDistance(data.at("settings").at("renderDistance").get<float>());
	GameVoxelChunk::meshmerizerIntensity = data.at("settings").at("meshmerizeEffect").get<float>();
	_directionLightNode->light()->cast_shadow = data.at("settings").at("castShadow").get<bool>();

	_renderDistanceSlider->set_value((_dynamicWorld->getRenderDistance() - DynamicWorld::minRenderDistance) / (DynamicWorld::maxRenderDistance - DynamicWorld::minRenderDistance));
	_renderDistanceValue->set_value(_dynamicWorld->getRenderDistance());
	_meshmerizerSlider->set_value(GameVoxelChunk::meshmerizerIntensity);
	_meshmerizerValue->set_value(GameVoxelChunk::meshmerizerIntensity);
	_directionalLightCheckbox->set_checked(_directionLightNode->light()->cast_shadow);
	_seedField->set_value(_dynamicWorld->getGenerator()->getSeed());
	_generatorKernelField->set_value(data.at("settings").at("generator").get<std::string>());

	_dynamicWorld->setWorldName(data.at("worldName").get<std::string>());
}

void VoxelProceduralSceneController::_toggleCinematicMode() {
	_axesNode->setActive(_cinematicMode);
	_handBlock->node->setActive(_cinematicMode);
	_placeHolderBlockOfDoom->setActive(_cinematicMode);
	_displayInterface = _cinematicMode;
	updateUI();
	_cinematicMode = !_cinematicMode;
}

void VoxelProceduralSceneController::_updateHandBlock() {
	_handBlock->updateMesh();
	_handBlock->mesh->generateBuffers();
}

std::vector<std::pair<std::string, GLS::VoxelBlock> > VoxelProceduralSceneController::_pickableBlocks = {
	std::make_pair("Bedrock", 		GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_BEDROCK)),
	std::make_pair("Stone", 		GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_STONE)),
	std::make_pair("Dirt", 			GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_DIRT)),
	std::make_pair("Grass", 		GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_GRASS)),
	std::make_pair("Sand", 			GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_SAND)),
	std::make_pair("Gravel", 		GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_GRAVEL)),
	std::make_pair("Clay",	 		GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_CLAY)),
	std::make_pair("Brown Grass", 	GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_GRASS_BROWN)),
	std::make_pair("Wood", 			GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_WOOD)),
	std::make_pair("Leaf", 			GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_LEAFS_TREE)),
	std::make_pair("Mountain Leaf", GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_LEAFS_MOUNTAIN)),
	std::make_pair("Oak Plank", 	GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_WOOD_PLANKS)),
	std::make_pair("Brick", 		GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_BRICKS)),
	std::make_pair("Cobblestone", 	GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_COBBLESTONE)),
	std::make_pair("Sandstone", 	GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_SANDSTONE)),
	std::make_pair("Terracotta", 	GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_TERRACOTTA_BROWN)),
	std::make_pair("Terracotta", 	GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_TERRACOTTA_YELLOW)),
	std::make_pair("Terracotta", 	GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_TERRACOTTA_ORANGE)),
	std::make_pair("Ice", 			GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_ICE)),
	std::make_pair("Broken Ice", 	GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_ICE_BROKEN)),
	std::make_pair("Snow",	 		GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_SNOW)),
	std::make_pair("Obsidian", 		GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_OBSIDIAN)),
	std::make_pair("Purple Grass", 	GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_GRASS_PURPLE)),
	std::make_pair("Gold", 			GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_GOLD)),
	std::make_pair("TNT", 			GLS::VoxelBlock(GLS::VoxelBlockMeshType::Full, BLOCK_TNT)),
	std::make_pair("Bush",			GLS::VoxelBlock(GLS::VoxelBlockMeshType::Cross, BLOCK_BUSH)),
	std::make_pair("Dead bush",	 	GLS::VoxelBlock(GLS::VoxelBlockMeshType::Cross, BLOCK_BUSH_DEAD)),
	std::make_pair("Flower",	 	GLS::VoxelBlock(GLS::VoxelBlockMeshType::Cross, BLOCK_FLOWER)),
	std::make_pair("Red Mushroom", 	GLS::VoxelBlock(GLS::VoxelBlockMeshType::Cross, BLOCK_MUSHROOM_RED)),
	std::make_pair("Brown Mushroom",GLS::VoxelBlock(GLS::VoxelBlockMeshType::Cross, BLOCK_MUSHROOM_BROWN)),
	std::make_pair("Cactus",		GLS::VoxelBlock(GLS::VoxelBlockMeshType::Curved, BLOCK_CACTUS)),
	std::make_pair("Water", 		GLS::VoxelBlock(GLS::VoxelBlockMeshType::ReduceHeight, BLOCK_WATER)),
};
