
#pragma once

#include "AppEnv.hpp"

class Human;
struct HumanAnimationState;

class HumanSceneController: public ISceneController {
    std::shared_ptr<Human> hooman;
    std::shared_ptr<GLS::Animator<HumanAnimationState> > hoomanAnimator;
    public:
    HumanSceneController(std::shared_ptr<GLSWindow> window);
    virtual ~HumanSceneController();

    virtual void makeScene();
    virtual void update();
};
