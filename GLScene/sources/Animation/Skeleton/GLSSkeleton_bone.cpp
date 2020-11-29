//
//  GLSSkeleton_bone.cpp
//  GLScene
//
//  Created by Arthur Masson on 28/11/2020.
//  Copyright © 2020 Arthur Masson. All rights reserved.
//

#include "GLSSkeleton.hpp"
#include "GLSNode.hpp"

namespace GLS {

    Skeleton::Bone::Bone(void *boneNode) :
    node(boneNode),
    restPosition(static_cast<Node*>(boneNode)->transform().matrix())
    {}

    Skeleton::Bone::Bone(void *boneNode, glm::mat4 boneRestPosition) :
    node(boneNode),
    restPosition(boneRestPosition)
    {}

    void Skeleton::addRootBone(void *node) {
        addBone(node);
        _rootBoneIndex = _bones.size() - 1;
    }

    void Skeleton::addRootBone(void *node, glm::mat4 rest) {
        addBone(node, rest);
        _rootBoneIndex = _bones.size() - 1;
    }

    void Skeleton::addBone(void *node) {
        Bone bone(node);
        _bones.push_back(bone);
    }

    void Skeleton::addBone(void *node, glm::mat4 rest) {
        Bone bone(node, rest);
        _bones.push_back(bone);
    }

    int Skeleton::boneCount() const {
        return _bones.size();
    }

    int Skeleton::indexOfBoneNamed(std::string name) const {
        for (size_t i = 0; i < _bones.size(); i++) {
            if (_bones[i].node != nullptr && static_cast<Node*>(_bones[i].node)->name() == name) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

}
