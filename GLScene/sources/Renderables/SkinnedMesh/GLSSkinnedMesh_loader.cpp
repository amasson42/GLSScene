//
//  GLSSkinnedMesh_load.cpp
//  GLScene
//
//  Created by Arthur Masson on 26/06/2019.
//  Copyright © 2019 Arthur Masson. All rights reserved.
//

#include "GLSStructs.hpp"
#include "GLSSkinnedMesh.hpp"

namespace GLS {

    std::shared_ptr<SkinnedMesh> SkinnedMesh::loadFromAiMesh(const aiMesh *mesh, std::shared_ptr<Skeleton> skeleton, std::shared_ptr<Node> rootNode, bool generateBuffers) {
        std::shared_ptr<SkinnedMesh> skMesh = std::make_shared<SkinnedMesh>();
        skMesh->_skeleton = skeleton;

        skMesh->_rootNode = rootNode;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            SkinnedVertex v;
            v.position = aiToGlm(mesh->mVertices[i]);
            if (mesh->HasNormals()) {
                v.normal = aiToGlm(mesh->mNormals[i]);
            }
            if (mesh->mTextureCoords[0]) {
                v.uv.x = mesh->mTextureCoords[0][i].x;
                v.uv.y = mesh->mTextureCoords[0][i].y;
            }
            if (mesh->HasTangentsAndBitangents()) {
                v.tangent = aiToGlm(mesh->mTangents[i]);
                v.bitangent = aiToGlm(mesh->mBitangents[i]);
            } else
                v = SkinnedVertex(v.position, v.normal, v.uv);
            skMesh->verticesRef().push_back(v);
        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                skMesh->indicesRef().push_back(face.mIndices[j]);
            }
        }
        int joint_overflow = 0;
        for (unsigned int i = 0; i < mesh->mNumBones; i++) {
            std::string boneName(mesh->mBones[i]->mName.C_Str());
            int boneIndex = skeleton->indexOfBoneNamed(boneName);
            if (boneIndex < 0) {
                std::cerr << ">>>>" << std::endl;
                std::cerr << "Located unexisting bone !" << std::endl;
                std::cerr << "BoneName = " << boneName << std::endl;
                for (size_t i = 0; i < skeleton->bones().size(); i++) {
                    std::cerr << skeleton->bones()[i].node.lock()->name() << std::endl;
                }
                std::cerr << "<<<<" << std::endl;
                return nullptr;
            }
            for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
                aiVertexWeight weight = mesh->mBones[i]->mWeights[j];
                if (!skMesh->verticesRef()[weight.mVertexId].addWeight(boneIndex, weight.mWeight))
                    joint_overflow++;
            }
        }
        if (joint_overflow > 0) {
            std::cerr << rootNode->name() << "-joints_overflows=" << joint_overflow << std::endl;
        }
        for (size_t i = 0; i < skMesh->verticesRef().size(); i++) {
            skMesh->verticesRef()[i].balanceWeights();
            // glm::vec4& v(skMesh->verticesRef()[i].joint_weights);
            // v = v / (v.x + v.y + v.z + v.w);
        }
        if (generateBuffers)
            skMesh->generateBuffers();
        return skMesh;
    }

}
