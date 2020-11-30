//
//  GLSTransformInterpolator_load.cpp
//  GLScene
//
//  Created by Arthur Masson on 23/11/2020.
//  Copyright © 2020 Arthur Masson. All rights reserved.
//

#include "GLSStructs.hpp"
#include "GLSInterpolator.hpp"

namespace GLS {

    std::shared_ptr<TransformInterpolator> TransformInterpolator::loadFromAiNodeAnim(const aiNodeAnim* anim) {
        std::shared_ptr<TransformInterpolator> ti = std::make_shared<TransformInterpolator>();

        for (unsigned int i = 0; i < anim->mNumPositionKeys; i++) {
            aiVectorKey posKey = anim->mPositionKeys[i];
            timefloat time = static_cast<timefloat>(posKey.mTime);
            glm::vec3 pos = aiToGlm(posKey.mValue);
            ti->addPositionAt(time, pos);
        }

        for (unsigned int i = 0; i < anim->mNumRotationKeys; i++) {
            aiQuatKey rotKey = anim->mRotationKeys[i];
            timefloat time = static_cast<timefloat>(rotKey.mTime);
            glm::quat rot = aiToGlm(rotKey.mValue);
            ti->addRotationAt(time, rot);
        }

        for (unsigned int i = 0; i < anim->mNumScalingKeys; i++) {
            aiVectorKey scaleKey = anim->mScalingKeys[i];
            timefloat time = static_cast<timefloat>(scaleKey.mTime);
            glm::vec3 scale = aiToGlm(scaleKey.mValue);
            ti->addScaleAt(time, scale);
        }

        return ti;
    }

}