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

        virtual void initAnimation();
        virtual void animate(timefloat deltaTime);
        virtual bool alive() const;
    };

    class Skeleton : public IAnimatable {
        
    public:

        static const int maxBones = 64;

        struct Bone {
            std::weak_ptr<Node> node;
            glm::mat4 offset;
            glm::mat4 globalRestPosition;

            Bone(std::shared_ptr<Node> node);
        };

    protected:

        std::vector<Bone> _bones;

        std::map<std::string, SkeletonAnimation> _animations;
        std::map<std::string, SkeletonAnimation>::iterator _currentAnimation;

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
        void addAnimationWithName(std::string name, const SkeletonAnimation& animation);

        std::string currentAnimationName() const;
        timefloat currentAnimationTime() const;

        virtual void initAnimation();
        virtual void initAnimationNamed(std::string name, timefloat offsetTime = 0);
        virtual void animate(timefloat deltaTime);
        virtual bool alive() const;

    };

}

#endif /* GLSSkeleton_h */
