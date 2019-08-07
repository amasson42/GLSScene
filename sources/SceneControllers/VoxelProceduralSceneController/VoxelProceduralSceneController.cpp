
#include "AppEnv.hpp"

VoxelProceduralSceneController::VoxelProceduralSceneController(std::shared_ptr<GLSWindow> window):
ISceneController(window) {
	lt = 0;
	cameraMoveSpeed = 5;
	_nanoguiScreen = _window.lock()->nanoguiScreen();
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
		
		// Update the ui if needed
		if (_displayInterface) {
			_fpsValueLabel->setCaption(std::to_string(static_cast<int>(1.0f / _window.lock()->deltaTime())));
			glm::vec3 playerPosition = cameraNode->transform().position();
			_playerPositionLabel[0]->setValue(static_cast<int>(playerPosition.x));
			_playerPositionLabel[1]->setValue(static_cast<int>(playerPosition.y));
			_playerPositionLabel[2]->setValue(static_cast<int>(playerPosition.z));

			_speedValueField->setValue(cameraMoveSpeed);

			_nanoguiScreen->performLayout();
		}
	}

	{
		glm::vec3 axesOffset = cameraNode->transform().matrix() * glm::vec4(0, 0, -2, 1);
		_axesNode->transform().setPosition(axesOffset);
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
	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		_displayInterface = !_displayInterface;
		updateUI();
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

	// World
	worldNode = std::make_shared<GLS::Node>();
	scene.rootNode()->addChildNode(worldNode);
	_dynamicWorld = std::make_shared<DynamicWorld>(worldNode, "world");
	_dynamicWorld->getGenerator()->usedMaterial = texturedMaterial;

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
	scene.setBackgroundColor(glm::vec4(0.3, 0.3, 0.3, 1));
	camera->aspect = (scene.getAspect());
	cameraNode->setCamera(camera);
	scene.setCameraNode(cameraNode);
	scene.rootNode()->addChildNode(cameraNode);
	cameraNode->addChildNode(pointlightNode);

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

	// Create a window, the draggable panel

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
		this->cameraNode->transform().position().x = posX;
	});
	_playerPositionLabel[1]->setCallback([this](int posY) {
		this->cameraNode->transform().position().y = posY;
	});
	_playerPositionLabel[2]->setCallback([this](int posZ) {
		this->cameraNode->transform().position().z = posZ;
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

	const float minFov = M_PI * 0.16666f;
	const float maxFov = M_PI * 0.83333f;
	auto fovBox = new nanogui::Widget(_playerWindow);
	fovBox->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	new nanogui::Label(fovBox, "Fov: ");
	auto fovSlider = new nanogui::Slider(fovBox);
	fovSlider->setValue((camera->fov - minFov) / (maxFov - minFov));
	auto fovValue = new nanogui::FloatBox<float>(fovBox, camera->fov * 180.0f / M_PI);
	fovSlider->setCallback([this, camera, fovValue, minFov, maxFov](float value) {
		camera->fov = value * (maxFov - minFov) + minFov;
		fovValue->setValue(camera->fov * 180.0f / M_PI);
	});

	// Environement GUI

	_environementWindow = new nanogui::Window(_nanoguiScreen, "Environement");
	_environementWindow->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Vertical));

	auto renderDistanceWidget = new nanogui::Widget(_environementWindow);
	renderDistanceWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	new nanogui::Label(renderDistanceWidget, "Render distance: ");
	auto renderDistanceSlider = new nanogui::Slider(renderDistanceWidget);

	auto renderDistanceValue = new nanogui::IntBox<int>(renderDistanceWidget, _dynamicWorld->getRenderDistance());
	renderDistanceSlider->setCallback([this, renderDistanceValue, camera](float value) {
		float renderDistance = value * (DynamicWorld::maxRenderDistance - DynamicWorld::minRenderDistance)
								+ DynamicWorld::minRenderDistance;
		renderDistanceValue->setValue(renderDistance);
		_dynamicWorld->setRenderDistance(renderDistance);
		camera->farZ = renderDistance;
		camera->fogFar = renderDistance;
		camera->fogNear = renderDistance / 2;
	});
	renderDistanceSlider->setValue((_dynamicWorld->getRenderDistance() - DynamicWorld::minRenderDistance)
									/ (DynamicWorld::maxRenderDistance - DynamicWorld::minRenderDistance));

	auto applyButtonWidget = new nanogui::Widget(_environementWindow);
	applyButtonWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal));
	auto applyButton = new nanogui::Button(applyButtonWidget, "Boutton inutile");
	applyButton->setCallback([]() {
		std::cout << "Je ne sert a rien" << std::endl;
	});
	// _dynamicWorld->setRenderDistance(renderDistance);

	updateUI();

	// Compute the layout (width, height)
	_nanoguiScreen->performLayout();

	_environementWindow->setPosition(nanogui::Vector2i(window->size().x - _environementWindow->width() - 10, 10));

}
