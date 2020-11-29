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
            void *node;
            glm::mat4 restPosition;

            Bone(void *node = nullptr);
            Bone(void *node, glm::mat4 restPosition);
        };

    protected:

        int _rootBoneIndex;
        std::vector<Bone> _bones;

        std::map<std::string, SkeletonAnimation> _animations;
        std::map<std::string, SkeletonAnimation>::iterator _currentAnimation;

    public:

        Skeleton();
        Skeleton(const Skeleton& copy);
        virtual ~Skeleton();

        Skeleton& operator=(const Skeleton& copy);

        static std::shared_ptr<Skeleton> loadFromAiSceneAnimations(const aiScene *scene, void *hostNode);

        // bones

        void addRootBone(void *node);
        void addRootBone(void *node, glm::mat4 rest);
        void addBone(void *node);
        void addBone(void *node, glm::mat4 rest);

        int boneCount() const;
        int indexOfBoneNamed(std::string name) const;

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
