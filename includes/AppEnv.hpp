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

    std::vector<std::string> args;

    std::shared_ptr<GLSWindow> mainWindow;
    std::shared_ptr<ISceneController> sceneController;

    AppEnv(const std::vector<std::string>& as);

    virtual ~AppEnv();

    std::shared_ptr<std::string> getArgument(std::string key) const;
    bool hasArgument(std::string key) const;

    void loop();
};
