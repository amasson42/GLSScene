
#pragma once

#include "AppEnv.hpp"

#include "BigChunk.hpp"
#include "DynamicWorld.hpp"
#include "ProceduralWorldGenerator.hpp"


#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/checkbox.h>
#include <nanogui/label.h>
#include <nanogui/layout.h>
#include <nanogui/textbox.h>


int* initNoise(unsigned int seed);
double smoothNoise(double x, double y, double z);
double linearNoise(double x, double y, double z);

class DynamicWorld;
class VoxelProceduralSceneController: public ISceneController {
    std::shared_ptr<GLS::Node> worldNode;
    std::shared_ptr<GLS::Node> cameraNode;
    double lt;
	std::shared_ptr<DynamicWorld> _dynamicWorld;

public:
    VoxelProceduralSceneController(std::shared_ptr<GLSWindow> window);
    virtual ~VoxelProceduralSceneController();

    virtual void makeScene();
    virtual void update();
    virtual void keyCallBack(int key, int scancode, int action, int mods);

	nanogui::Screen* getScreen();

private:
	nanogui::Screen* _nanoguiScreen;
	nanogui::Label* _fpsValueLabel;
	nanogui::IntBox<int>* _playerPositionLabel[3];

	bool _displayInterface;

};
