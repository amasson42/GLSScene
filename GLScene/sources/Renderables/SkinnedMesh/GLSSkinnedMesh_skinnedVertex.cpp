//
//  GLSSkinnedMesh_skinnedVertex.cpp
//  GLScene
//
//  Created by Arthur Masson on 26/06/2019.
//  Copyright © 2019 Arthur Masson. All rights reserved.
//

#include "GLSSkinnedMesh.hpp"

namespace GLS {

    SkinnedVertex::SkinnedVertex() {
        position = glm::vec3(0);
        normal = glm::vec3(0, 0, -1);
        tangent = glm::vec3(1, 0, 0);
        bitangent = glm::vec3(0, 1, 0);
        uv = glm::vec2(0);
        joint_ids = glm::ivec4(0, 0, 0, 0);
        joint_weights = glm::vec4(0, 0, 0, 0);
    }
    
    SkinnedVertex::SkinnedVertex(const glm::vec3 p,
        const glm::vec3 n,
        const glm::vec3 t,
        const glm::vec3 b,
        const glm::vec2 u) {
        position = p;
        normal = n;
        tangent = t;
        bitangent = b;
        uv = u;
        joint_ids = glm::ivec4(0, 0, 0, 0);
        joint_weights = glm::vec4(0, 0, 0, 0);
    }
    
    SkinnedVertex::SkinnedVertex(const glm::vec3& o,
        const glm::vec3& n,
        const glm::vec2& u) {
        position = o;
        normal = n;
        if (n == glm::vec3(0, 1, 0))
            tangent = glm::vec3(1, 0, 0);
        else if (n == glm::vec3(0, -1, 0))
            tangent = glm::vec3(-1, 0, 0);
        else
            tangent = glm::cross(glm::vec3(0, 1, 0), n);
        bitangent = glm::cross(tangent, n);
        uv = u;
        joint_ids = glm::ivec4(0, 0, 0, 0);
        joint_weights = glm::vec4(0, 0, 0, 0);
    }

    bool SkinnedVertex::addWeight(int id, float weight) {
        int mini = 0;
        for (int i = 0; i < 4; i++) {
            if (joint_weights[i] == 0) {
                joint_ids[i] = id;
                joint_weights[i] = weight;
                return true;
            }
            if (joint_weights[i] < joint_weights[mini])
                mini = i;
        }
        joint_ids[mini] = id;
        joint_weights[mini] = weight;
        return false;
    }

    void SkinnedVertex::balanceWeights() {
        glm::vec4& v(joint_weights);
        v = v / (v.x + v.y + v.z + v.w);
    }

}
