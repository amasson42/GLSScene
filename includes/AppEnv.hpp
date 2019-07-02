//
//  AppEnv.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#pragma once

struct AppEnv;
class GLSWindow;
class ISceneController;

# include "GLSWindow.hpp"
# include "ISceneController.hpp"

struct AppEnv {

    static void printAvailableScenes();

    std::vector<std::string> args;

    std::vector<std::shared_ptr<GLSWindow> > windows;
    std::shared_ptr<ISceneController> sceneController;

    AppEnv(const std::vector<std::string>& as);

    virtual ~AppEnv();

    std::shared_ptr<std::string> getArgument(std::string key) const;

    void loop();
};
