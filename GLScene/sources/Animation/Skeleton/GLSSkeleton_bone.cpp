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

    Skeleton::Bone::Bone(std::shared_ptr<Node> boneNode,
        glm::mat4 boneRestPosition,
        glm::mat4 boneInverseBindPosition) :
    node(boneNode),
    restPosition(boneRestPosition),
    inverseBindPosition(boneInverseBindPosition)
    {}

    void Skeleton::addBone(std::shared_ptr<Node> node) {
        glm::mat4 rest;
        glm::mat4 bind;
        if (_bones.empty()) {
            rest = glm::mat4(1);
            bind = glm::mat4(1);
        } else {
            rest = node->getTransformMatrix();
            bind = glm::inverse(node->getParentNodeRelativeTransformMatrix(_bones[0].node.lock()));
        }
        Bone bone(node, rest, bind);
        bone.offset = glm::mat4(1);
        _bones.push_back(bone);
    }

    void Skeleton::addBone(std::shared_ptr<Node> node, glm::mat4 offset) {
        glm::mat4 bind;
        if (_bones.empty()) {
            bind = glm::mat4(1);
        } else {
            // glm::mat4 tmpPose = node->getTransformMatrix();
            // node->transform().setMatrix(rest);
            bind = glm::inverse(node->getParentNodeRelativeTransformMatrix(_bones[0].node.lock()));
            // node->transform().setMatrix(tmpPose);
        }
        Bone bone(node, offset, bind);
        bone.offset = offset;
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
