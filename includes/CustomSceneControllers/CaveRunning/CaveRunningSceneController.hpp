
#pragma once

#include "AppEnv.hpp"

#include "CaveMaze.hpp"

class CaveRunningSceneController : public ISceneController {

    public:
    CaveRunningSceneController(std::shared_ptr<GLSWindow> window);
    virtual ~CaveRunningSceneController();

    virtual void makeScene();
    virtual void update();
    virtual void keyCallBack(int key, int scancode, int action, int mods);
    virtual void scrollCallBack(double x, double y);
    virtual void resizeWindowCallBack(glm::vec2 newSize);
    virtual void mouseButtonCallBack(int button, int action, int modifiers);
	virtual void closeCallback();
};
