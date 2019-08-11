
#pragma once

#include "AppEnv.hpp"

#include "BigChunk.hpp"
#include "DynamicWorld.hpp"
#include "ProceduralWorldGenerator.hpp"

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

    void updateUI();

private:

    std::shared_ptr<GLS::Node> worldNode;
    std::shared_ptr<GLS::Node> cameraNode;
    std::shared_ptr<GLS::Node> _axesNode;
    double lt;
	std::shared_ptr<DynamicWorld> _dynamicWorld;

	nanogui::Screen* _nanoguiScreen;

    // Player GUI
    nanogui::Window* _playerWindow;
	nanogui::Label* _fpsValueLabel;
	nanogui::IntBox<int>* _playerPositionLabel[3];
    nanogui::FloatBox<float>* _speedValueField;
    nanogui::Label* _pickedBlockLabel;
    nanogui::ImageView* _pickedBlockTexture;
    std::shared_ptr<GameVoxelChunk> _handBlock;
    std::shared_ptr<GLS::Node> _placeHolderBlockOfDoom;

    // Environement GUI
    nanogui::Window* _environementWindow;
    nanogui::TextBox* _generatorKernelField;

	bool _displayInterface;

    int _pickedBlock;
    
    static const std::map<int, std::string> _BlockNames;

};
