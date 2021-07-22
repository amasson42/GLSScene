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
    _activeAnimations() {

    }

    Skeleton::Skeleton(const Skeleton& copy) :
    _bones(copy._bones),
    _animations(copy._animations),
    _activeAnimations() {
        for (size_t i = 0; i < copy._activeAnimations.size(); i++) {
            std::shared_ptr<AnimationBlender> anim = copy._activeAnimations[i];
            setAnimationBlend(anim->iterator->first, anim->iterator->second._currentTime, anim->blendValue);
        }
    }

    Skeleton::~Skeleton() {

    }

    Skeleton& Skeleton::operator=(const Skeleton& copy) {
        _bones = copy._bones;
        _animations = copy._animations;
        _activeAnimations.clear();
        for (size_t i = 0; i < copy._activeAnimations.size(); i++) {
            std::shared_ptr<AnimationBlender> anim = copy._activeAnimations[i];
            setAnimationBlend(anim->iterator->first, anim->iterator->second._currentTime, anim->blendValue);
        }
        return *this;
    }

}
