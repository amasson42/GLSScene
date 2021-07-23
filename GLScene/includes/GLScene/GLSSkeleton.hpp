//
//  GLSShader.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSSkeleton_h
#define GLSSkeleton_h

#include "GLSStructs.hpp"
#include "GLSIAnimatable.hpp"
#include "GLSInterpolator.hpp"
#include "GLSNode.hpp"

namespace GLS {

    class SkeletonAnimation : public IAnimatable {
        std::vector<TransformInterpolator> _interpolations;
        timefloat _currentTime;
        timefloat _duration;
        bool _loop;

        friend class Skeleton;

        public:

        SkeletonAnimation();
        SkeletonAnimation(const SkeletonAnimation& copy);
        virtual ~SkeletonAnimation();

        SkeletonAnimation& operator=(const SkeletonAnimation& copy);

        TransformInterpolator& interpolatorAt(int boneIndex);

        timefloat duration() const;
        void setDuration(timefloat d);
        void setMinDuration(timefloat d);
        void setMaxDuration(timefloat d);

        bool loop() const;
        void setLoop(bool l);

        timefloat currentTime() const;

        virtual void initAnimation();
        virtual void animate(timefloat deltaTime);
        virtual bool alive() const;
    };

    struct AnimationBlender {
        std::map<std::string, SkeletonAnimation>::iterator iterator;
        float blendValue;
    };

    class Skeleton : public IAnimatable {
        
    public:

        static const int maxBones = 64;

        struct Bone {
            std::weak_ptr<Node> node;
            glm::mat4 inverseBind;
            Transform restPose;

            Bone(std::shared_ptr<Node> node);
        };

    protected:

        std::vector<Bone> _bones;
        std::map<std::string, SkeletonAnimation> _animations;
        std::vector<std::shared_ptr<AnimationBlender>> _activeAnimations;

    public:

        Skeleton();
        Skeleton(const Skeleton& copy);
        virtual ~Skeleton();

        Skeleton& operator=(const Skeleton& copy);

        static std::shared_ptr<Skeleton> loadFromAiSceneAnimations(const aiScene *scene, std::shared_ptr<Node> hostNode);

        // bones

        void addBone(std::shared_ptr<Node> node);
        void addBone(std::shared_ptr<Node> node, glm::mat4 offset);
        int boneCount() const;
        int indexOfBoneNamed(std::string name) const;
        const std::vector<Bone>& bones() const;

        // animations

        std::vector<std::string> animationNames() const;
        SkeletonAnimation *createAnimation(std::string name, timefloat duration, bool loop);

        std::map<std::string, float> getAnimationBlends() const;
        timefloat getAnimationTime(std::string name) const;

        virtual void initAnimation();
        virtual std::shared_ptr<AnimationBlender> setAnimationBlend(std::string name, timefloat offsetTime = 0, float blend = 1.0f);
        virtual void stopAnimations();
        virtual void animate(timefloat deltaTime);
        virtual bool alive() const;

    };

}

#endif /* GLSSkeleton_h */
