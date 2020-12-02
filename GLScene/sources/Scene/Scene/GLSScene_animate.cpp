//
//  GLSScene.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSScene.hpp"

namespace GLS {

    void Scene::updateAnimations(timefloat deltaTime) {
        _rootNode->animate(deltaTime, true);
    }

}
