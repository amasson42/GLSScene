
#pragma once

#include "AppEnv.hpp"

class TrashSceneController: public ISceneController {
    public:
    TrashSceneController(std::shared_ptr<GLSWindow> window);
    virtual ~TrashSceneController();

    virtual void makeScene();
    virtual void update();
};
