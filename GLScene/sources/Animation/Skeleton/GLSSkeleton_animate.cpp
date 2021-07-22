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

    TransformInterpolator& SkeletonAnimation::interpolatorAt(int boneIndex) {
        return _interpolations[boneIndex];
    }

    timefloat SkeletonAnimation::duration() const {
        return _duration;
    }

    void SkeletonAnimation::setDuration(timefloat d) {
        _duration = d;
    }

    void SkeletonAnimation::setMinDuration(timefloat d) {
        _duration = d;
        for (size_t i = 0; i < _interpolations.size(); i++) {
            _duration = std::max(_interpolations[i].duration(), _duration);
        }
    }

    void SkeletonAnimation::setMaxDuration(timefloat d) {
        _duration = d;
        for (size_t i = 0; i < _interpolations.size(); i++) {
            _duration = std::min(_interpolations[i].duration(), _duration);
        }
    }

    bool SkeletonAnimation::loop() const {
        return _loop;
    }

    void SkeletonAnimation::setLoop(bool l) {
        _loop = l;
    }

    timefloat SkeletonAnimation::currentTime() const {
        return _currentTime;
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
        return _loop || _currentTime <= _duration;
    }


    // Skeleton

    std::vector<std::string> Skeleton::animationNames() const {
        std::vector<std::string> names;
        for (auto it = _animations.begin(); it != _animations.end(); ++it) {
            names.push_back(it->first);
        }
        return names;
    }

    SkeletonAnimation *Skeleton::createAnimation(std::string name, timefloat duration, bool loop) {
        SkeletonAnimation animation;
        animation._duration = duration;
        animation._loop = loop;
        animation._interpolations = std::vector<TransformInterpolator>(_bones.size());
        _animations[name] = animation;
        return &(_animations[name]);
    }

    std::map<std::string, float> Skeleton::getAnimationBlends() const {
        std::map<std::string, float> blends;
        for (size_t i = 0; i < _activeAnimations.size(); i++) {
            blends[_activeAnimations[i]->iterator->first] = _activeAnimations[i]->blendValue;
        }
        return blends;
    }

    timefloat Skeleton::getAnimationTime(std::string name) const {
        auto it = _animations.find(name);
        if (it != _animations.end())
            return it->second._currentTime;
        return 0;
    }

    void Skeleton::initAnimation() {
        for (auto it = _animations.begin(); it != _animations.end(); ++it) {
            it->second.initAnimation();
        }
    }

    std::shared_ptr<AnimationBlender> Skeleton::setAnimationBlend(std::string name, timefloat offsetTime, float blend) {
        std::shared_ptr<AnimationBlender> blender = nullptr;
        // Look for an existing blend animation
        for (size_t i = 0; i < _activeAnimations.size(); i++) {
            blender = _activeAnimations[i];
            if (blender->iterator->first == name) {
                if (blend <= 0.0) {
                    _activeAnimations.erase(_activeAnimations.begin() + i);
                    return nullptr;
                }
                blender->iterator->second.initAnimation();
                blender->iterator->second._currentTime = offsetTime;
                blender->blendValue = blend;
                return blender;
            }
        }
        if (blend <= 0.0)
            return nullptr;
        // Look for the existing animation
        for (auto iterator = _animations.begin(); iterator != _animations.end(); ++iterator) {
            if (iterator->first == name) {
                blender = std::make_shared<AnimationBlender>();
                blender->iterator = iterator;
                blender->iterator->second.initAnimation();
                blender->iterator->second._currentTime = offsetTime;
                blender->blendValue = blend;
                _activeAnimations.push_back(blender);
                return blender;
            }
        }
        // The name is not refering to an existing animation
        return nullptr;
    }

    void Skeleton::stopAnimations() {
        _activeAnimations.clear();
    }

    void Skeleton::animate(timefloat deltaTime) {
        for (size_t i = 0; i < _activeAnimations.size(); i++)
            _activeAnimations[i]->iterator->second.animate(deltaTime);
        for (size_t boneIndex = 0; boneIndex < _bones.size(); boneIndex++) {
            if (_bones[boneIndex].node.expired() == false) {
                std::shared_ptr<Node> node = _bones[boneIndex].node.lock();
                
                Transform totalTransform;
                float totalBlend = 0;
                for (size_t j = 0; j < _activeAnimations.size(); j++) {
                    SkeletonAnimation animation = _activeAnimations[j]->iterator->second;
                    float blendValue = _activeAnimations[j]->blendValue;
                    TransformInterpolator& interpolator(animation.interpolatorAt(boneIndex));
                    Transform frameTransform = interpolator.transformAt(animation.currentTime(), node->transform());
                    totalTransform.blendWith(frameTransform, totalBlend, blendValue);
                    totalBlend += blendValue;
                }
                if (1 - totalBlend >= 0) {
                    // totalTransform.blendWith(Transform(), totalBlend, 1 - totalBlend);
                    totalTransform.blendWith(node->transform(), totalBlend, 1 - totalBlend);
                }
                totalTransform.updateMatrix();
                node->setTransform(totalTransform);
            }
        }
/*
        if (_currentAnimation != _animations.end()) {
            _currentAnimation->second.animate(deltaTime);
            std::vector<TransformInterpolator>& interpolators(_currentAnimation->second._interpolations);
            timefloat time = _currentAnimation->second._currentTime;

            const int count = std::min(_bones.size(), interpolators.size());
            for (int i = 0; i < count; i++) {
                if (_bones[i].node.expired() == false) {
                    std::shared_ptr<GLS::Node> boneNode = _bones[i].node.lock();
                    GLS::Transform framePos = interpolators[i].transformAt(time);
                    if (!interpolators[i].keyPositions().isEmpty())
                        boneNode->transform().setPosition(framePos.position());
                    if (!interpolators[i].keyRotations().isEmpty())
                        boneNode->transform().setRotation(framePos.rotation());
                    if (!interpolators[i].keyScales().isEmpty())
                        boneNode->transform().setScale(framePos.scale());
                }
            }
        }
        */
    }

    bool Skeleton::alive() const {
        for (size_t i = 0; i < _activeAnimations.size(); i++) {
            if (_activeAnimations[i]->iterator->second.alive())
                return true;
        }
        return false;
    }

}
