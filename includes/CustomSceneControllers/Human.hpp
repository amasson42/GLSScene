
#pragma once

#include "AppEnv.hpp"

class HumanSceneController: public ISceneController {
    public:
    HumanSceneController(std::shared_ptr<GLSWindow> window);
    virtual ~HumanSceneController();

    virtual void makeScene();
    virtual void update();
};
