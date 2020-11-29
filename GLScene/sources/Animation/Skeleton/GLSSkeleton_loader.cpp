//
//  GLSSkeleton_loader.cpp
//  GLScene
//
//  Created by Arthur Masson on 28/11/2020.
//  Copyright © 2020 Arthur Masson. All rights reserved.
//

#include "GLSSkeleton.hpp"
#include "GLSNode.hpp"

namespace GLS {

    std::shared_ptr<Skeleton> Skeleton::loadFromAiSceneAnimations(const aiScene *scene, void *hostNode) {
        Node *node = static_cast<Node*>(hostNode);
        
        std::shared_ptr<Skeleton> skeleton = std::make_shared<Skeleton>();

        { // load bones
            std::set<std::string> existingBones;
            for(unsigned int i = 0; i < scene->mNumAnimations; i++) {
                aiAnimation *animation = scene->mAnimations[i];
                for (unsigned int j = 0; j < animation->mNumChannels; j++) {
                    std::string boneName = std::string(animation->mChannels[j]->mNodeName.data);
                    if (existingBones.find(boneName) == existingBones.end()) {
                        existingBones.insert(boneName);
                        std::shared_ptr<Node> boneNode = node->childNodeNamed(boneName, true);
                        skeleton->addBone(boneNode.get());
                    }
                }
            }
        }

        // load animations
        for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
            aiAnimation *animation = scene->mAnimations[i];

            SkeletonAnimation skAnimation;
            skAnimation._interpolations.resize(skeleton->boneCount());
            for (unsigned int j = 0; j < animation->mNumChannels; j++) {
                aiNodeAnim *chan = animation->mChannels[j];

                auto transpol = TransformInterpolator::loadFromAiNodeAnim(chan);
                int boneIndex = skeleton->indexOfBoneNamed(std::string(chan->mNodeName.data));
                skAnimation._interpolations[boneIndex] = *transpol;
                skAnimation._duration = std::max(skAnimation._duration, transpol->duration());
                skAnimation._loop = true;
            }
            skeleton->addAnimationWithName(std::string(animation->mName.data), skAnimation);
        }

        return skeleton;
    }

}
