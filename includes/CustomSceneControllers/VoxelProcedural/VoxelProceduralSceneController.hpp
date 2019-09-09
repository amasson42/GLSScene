
#pragma once

#include "AppEnv.hpp"

#include "BigChunk.hpp"
#include "DynamicWorld.hpp"
#include "ProceduralWorldGenerator.hpp"

#include <nlohmann/json.hpp>

#define CHUNKSIZE (static_cast<int>(GLS::VoxelChunk::chunkSize))

int* initNoise(unsigned int seed);
double smoothNoise(double x, double y, double z);
double linearNoise(double x, double y, double z);

class DynamicWorld;
struct GameVoxelChunk;

class VoxelProceduralSceneController: public ISceneController {

public:
    VoxelProceduralSceneController(std::shared_ptr<GLSWindow> window);
    virtual ~VoxelProceduralSceneController();

    virtual void makeScene();
    virtual void update();
    virtual void keyCallBack(int key, int scancode, int action, int mods);
    virtual void scrollCallBack(double x, double y);
    virtual void resizeWindowCallBack(glm::vec2 newSize);
    virtual void mouseButtonCallBack(int button, int action, int modifiers);
	virtual void closeCallback();

    void updateUI();
	static constexpr char worldDirPrefix[] = "worlds/world_";

private:

	enum DisplayedWindow {
		StartMenu,
		NewWorld,
		Game
	};

    std::shared_ptr<GLS::Node> worldNode;
    std::shared_ptr<GLS::Node> cameraNode;
	std::shared_ptr<GLS::Node> _lightNode;
	std::shared_ptr<GLS::Node> _directionLightNode;
	std::shared_ptr<GLS::Node> _ambiantlightNode;
    std::shared_ptr<GLS::Node> _axesNode;
    double lt;
	std::shared_ptr<DynamicWorld> _dynamicWorld;

	nanogui::Screen* _nanoguiScreen;

	// Menus GUI
	nanogui::Window* _selectWorldWindow;
	nanogui::Window* _newWorldWindow;

    // Player GUI
    nanogui::Window* _playerWindow;
	nanogui::Label* _fpsValueLabel;
	nanogui::IntBox<int>* _playerPositionLabel[3];
    nanogui::FloatBox<float>* _speedValueField;
	nanogui::Slider* _fovSlider;
	nanogui::FloatBox<float>* _fovValue;
    nanogui::Label* _pickedBlockLabel;
    nanogui::ImageView* _pickedBlockTexture;
    std::shared_ptr<GameVoxelChunk> _handBlock;
    std::shared_ptr<GLS::Node> _placeHolderBlockOfDoom;

    // Environement GUI
    nanogui::Window* _environementWindow;
	nanogui::Slider* _renderDistanceSlider;
	nanogui::FloatBox<float>* _renderDistanceValue;
	nanogui::Slider* _meshmerizerSlider;
	nanogui::FloatBox<float>* _meshmerizerValue;
	nanogui::CheckBox* _directionalLightCheckbox;
	nanogui::IntBox<unsigned int>* _seedField;
    nanogui::TextBox* _generatorKernelField;

	bool _displayInterface;
	bool _cinematicMode;
	DisplayedWindow _startupWindow;

    int _pickedBlockIndex;

	void _setupWorld();
	void _setupLights();
	void _setupCamera();
	void _setupGUI();

	void _createWorldsFolder();
	void _updateWorldFolder();
	void _saveJsonFileInfo();
	nlohmann::json _readJsonFileInfo(std::string fileName);
	void _loadJsonFileInfo(nlohmann::json);
	void _toggleCinematicMode();

	void _updateHandBlock();

	static std::vector<std::pair<std::string, GLS::VoxelBlock> > _pickableBlocks;
	static constexpr char generatorFilePath[] = "assets/voxelProceduralGeneratorSources/";
	static constexpr char defaultGeneratorFileName[] = "hillsGenerator.cl";
};
