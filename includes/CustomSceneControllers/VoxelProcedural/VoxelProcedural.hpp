
#pragma once

#include "AppEnv.hpp"

#include "BigChunk.hpp"
#include "DynamicWorld.hpp"
#include "ProceduralWorldGenerator.hpp"

int* initNoise(unsigned int seed);
double smoothNoise(double x, double y, double z);
double linearNoise(double x, double y, double z);

class DynamicWorld;
class VoxelProceduralSceneController: public ISceneController {

public:
    VoxelProceduralSceneController(std::shared_ptr<GLSWindow> window);
    virtual ~VoxelProceduralSceneController();

    virtual void makeScene();
    virtual void update();
    virtual void keyCallBack(int key, int scancode, int action, int mods);
    virtual void resizeWindowCallBack(glm::vec2 newSize);

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

    // Environement GUI
    nanogui::Window* _environementWindow;

	bool _displayInterface;

};
