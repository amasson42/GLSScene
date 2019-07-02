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

#ifndef M_PI
# define M_PI 3.14159265359
#endif
#ifndef M_PI_2
# define M_PI_2 3.14159265359 / 2.0
#endif

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
