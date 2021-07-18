//
//  GLSSkeleton_load.cpp
//  GLScene
//
//  Created by Arthur Masson on 28/11/2020.
//  Copyright © 2020 Arthur Masson. All rights reserved.
//

#include "GLSStructs.hpp"
#include "GLSSkeleton.hpp"
#include "GLSNode.hpp"

namespace GLS {

    std::shared_ptr<Skeleton> Skeleton::loadFromAiSceneAnimations(const aiScene *scene, std::shared_ptr<Node> hostNode) {
        
        std::shared_ptr<Skeleton> skeleton = std::make_shared<Skeleton>();

        { // load bones
            std::set<std::string> existingBones;

            existingBones.insert(hostNode->name());
            skeleton->addBone(hostNode);
            std::cout << " bone[0]-" << hostNode->name() << std::endl;
            std::cout << "  >" << hostNode->globalName() << std::endl;

            for (unsigned int k = 0; k < scene->mNumMeshes; k++) {
                const aiMesh *mesh = scene->mMeshes[k];
                if (mesh->HasBones() == false) continue;

                for (unsigned int i = 0; i < mesh->mNumBones; i++) {
                    const aiBone *bone = mesh->mBones[i];
                    std::string boneName = std::string(bone->mName.C_Str());
                    if (existingBones.find(boneName) == existingBones.end()) {
                        std::shared_ptr<Node> boneNode = hostNode->childNodeNamed(boneName, true);
                        if (boneNode == nullptr)
                            continue;
                        std::cout << " bone[" << existingBones.size() << "]-" << boneName << std::endl;
                        std::cout << "  >" << boneNode->globalName() << std::endl;
                        skeleton->addBone(boneNode, aiToGlm(bone->mOffsetMatrix));
                        existingBones.insert(boneName);
                    }
                }
            }
            // FIXME: This code is only here to check that all bones are existing !
            for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
                aiAnimation *animation = scene->mAnimations[i];
                std::cout << "animation with ticks " << animation->mTicksPerSecond << std::endl;
                for (unsigned int j = 0; j < animation->mNumChannels; j++) {
                    std::string boneName = std::string(animation->mChannels[j]->mNodeName.C_Str());
                    if (existingBones.find(boneName) == existingBones.end()) {
                        std::cout << "---- New bone from animations ----" << boneName << std::endl;
                        existingBones.insert(boneName);
                        std::shared_ptr<Node> boneNode = hostNode->childNodeNamed(boneName, true);
                        skeleton->addBone(boneNode);
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
                int boneIndex = skeleton->indexOfBoneNamed(std::string(chan->mNodeName.C_Str()));
                skAnimation._interpolations[boneIndex] = *transpol;
                skAnimation._duration = std::max(skAnimation._duration, transpol->duration());
                // skAnimation._loop |= animation->mChannels[j]->mPostState == aiAnimBehaviour_REPEAT;
                skAnimation._loop = true;
            }
            skeleton->addAnimationWithName(std::string(animation->mName.data), skAnimation);
        }

        return skeleton;
    }

}
