//
//  GLSScene.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSScene.hpp"

namespace GLS {

    void Scene::addAnimatable(std::shared_ptr<IAnimatable> animatable) {
        _animatables.insert(animatable);
    }

    void Scene::removeAnimatable(std::shared_ptr<IAnimatable> animatable) {
        _animatables.erase(animatable);
    }

    void Scene::updateAnimations(float deltaTime) {
        std::set<std::shared_ptr<IAnimatable> >::iterator it = _animatables.begin();
        while (it != _animatables.end()) {
            (*it)->animate(deltaTime);
            ++it;
        }
    }

}
