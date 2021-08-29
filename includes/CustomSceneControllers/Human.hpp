
#pragma once

#include "AppEnv.hpp"

class HumanSceneController: public ISceneController {
    public:
    HumanSceneController(std::shared_ptr<GLSWindow> window);
    virtual ~HumanSceneController();
    static std::shared_ptr<GLS::Node> generateTentacle();

    virtual void makeScene();
    virtual void update();
    virtual void keyCallBack(int key, int scancode, int action, int mods);

    private:

    std::shared_ptr<GLS::Node> _mainNode;

    void _generateUI();
    nanogui::Window* _toolsWindow;

};
