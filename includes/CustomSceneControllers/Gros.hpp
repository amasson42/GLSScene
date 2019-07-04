
#pragma once

#include "AppEnv.hpp"

class GrosSceneController: public ISceneController {
	public:
	GrosSceneController(std::shared_ptr<GLSWindow> window);
	virtual ~GrosSceneController();

	virtual void makeScene();
	virtual void update();
	// virtual void keyCallBack(int key, int scancode, int action, int mods);
};
