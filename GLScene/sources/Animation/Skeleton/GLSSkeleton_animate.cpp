//
//  GLSSkeleton_animate.cpp
//  GLScene
//
//  Created by Arthur Masson on 28/11/2020.
//  Copyright © 2020 Arthur Masson. All rights reserved.
//

#include "GLSSkeleton.hpp"
#include "GLSNode.hpp"

namespace GLS {

    SkeletonAnimation::SkeletonAnimation() :
    _interpolations(),
    _currentTime(0),
    _duration(0),
    _loop(false)
    {}

    SkeletonAnimation::SkeletonAnimation(const SkeletonAnimation& copy) :
    _interpolations(copy._interpolations),
    _currentTime(copy._currentTime),
    _duration(copy._duration),
    _loop(copy._loop)
    {}

    SkeletonAnimation::~SkeletonAnimation() {

    }

    SkeletonAnimation& SkeletonAnimation::operator=(const SkeletonAnimation& copy) {
        _interpolations = copy._interpolations;
        _currentTime = copy._currentTime;
        _duration = copy._duration;
        _loop = copy._loop;
        return *this;
    }

    void SkeletonAnimation::initAnimation() {
        _currentTime = 0;
    }

    void SkeletonAnimation::animate(timefloat deltaTime) {
        _currentTime += deltaTime;
        if (_currentTime >= _duration && _loop) {
            _currentTime -= _duration;
        }
    }

    bool SkeletonAnimation::alive() const {
        return !_loop || _currentTime <= _duration;
    }

    std::vector<std::string> Skeleton::animationNames() const {
        std::vector<std::string> names;
        for (auto it = _animations.begin(); it != _animations.end(); ++it) {
            names.push_back(it->first);
        }
        return names;
    }

    void Skeleton::addAnimationWithName(std::string name, const SkeletonAnimation& animation) {
        _animations[name] = animation;
    }

    std::string Skeleton::currentAnimationName() const {
        return _currentAnimation->first;
    }

    timefloat Skeleton::currentAnimationTime() const {
        if (_currentAnimation != _animations.end()) {
            return _currentAnimation->second._currentTime;
        } else {
            return 0;
        }
    }

    void Skeleton::initAnimation() {
        if (_currentAnimation == _animations.end())
            _currentAnimation = _animations.begin();
        if (_currentAnimation != _animations.end())
            _currentAnimation->second.initAnimation();
    }

    void Skeleton::initAnimationNamed(std::string name, timefloat offsetTime) {
        for (_currentAnimation = _animations.begin(); _currentAnimation != _animations.end(); ++_currentAnimation) {
            if (_currentAnimation->first == name) {
                _currentAnimation->second.initAnimation();
                _currentAnimation->second._currentTime = offsetTime;
                break;
            }
        }
    }

    void Skeleton::animate(timefloat deltaTime) {
        if (_currentAnimation != _animations.end()) {
            _currentAnimation->second.animate(deltaTime);
            std::vector<TransformInterpolator>& interpolators(_currentAnimation->second._interpolations);
            timefloat time = _currentAnimation->second._currentTime;

            const int count = std::min(_bones.size(), interpolators.size());
            for (int i = 0; i < count; i++) {
                if (_bones[i].node.expired() == false) {
                    _bones[i].node.lock()->setTransform(interpolators[i].transformAt(time));
                }
            }
        }
    }

    bool Skeleton::alive() const {
        if (_currentAnimation != _animations.end()) {
            return _currentAnimation->second.alive();
        } else {
            return false;
        }
    }

}