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

    Skeleton::Bone::Bone(std::shared_ptr<Node> boneNode) :
    node(boneNode),
    offset(1),
    globalRestPosition(1)
    {}

    void Skeleton::addBone(std::shared_ptr<Node> node) {
        Bone bone(node);
        bone.offset = glm::mat4(1);
        if (_bones.empty()) {
            bone.globalRestPosition = glm::mat4(1);
        } else {
            bone.globalRestPosition = node->getParentNodeRelativeTransformMatrix(_bones[0].node.lock());
        }
        _bones.push_back(bone);
    }

    void Skeleton::addBone(std::shared_ptr<Node> node, glm::mat4 offset) {
        Bone bone(node);
        bone.offset = offset;
        if (_bones.empty()) {
            bone.globalRestPosition = glm::mat4(1);
        } else {
            bone.globalRestPosition = node->getParentNodeRelativeTransformMatrix(_bones[0].node.lock());
        }
        _bones.push_back(bone);
    }

    int Skeleton::boneCount() const {
        return _bones.size();
    }

    int Skeleton::indexOfBoneNamed(std::string name) const {
        for (size_t i = 0; i < _bones.size(); i++) {
            if (_bones[i].node.expired() == false && _bones[i].node.lock()->name() == name) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    const std::vector<Skeleton::Bone>& Skeleton::bones() const {
        return _bones;
    }

}
