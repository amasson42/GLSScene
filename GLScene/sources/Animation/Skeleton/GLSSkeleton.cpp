//
//  GLSSkeleton.cpp
//  GLScene
//
//  Created by Arthur Masson on 28/11/2020.
//  Copyright © 2020 Arthur Masson. All rights reserved.
//

#include "GLSSkeleton.hpp"
#include "GLSNode.hpp"

namespace GLS {

    // skeleton

    Skeleton::Skeleton() :
    _bones(),
    _animations(),
    _currentAnimation() {
        _currentAnimation = _animations.end();
    }

    Skeleton::~Skeleton() {

    }

    Skeleton& Skeleton::operator=(const Skeleton& copy) {
        _bones = copy._bones;
        _animations = copy._animations;
        initAnimationNamed(copy.currentAnimationName(), copy.currentAnimationTime());
        return *this;
    }

}
