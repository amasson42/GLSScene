//
//  GLSSkinnedMesh_load.cpp
//  GLScene
//
//  Created by Arthur Masson on 26/06/2019.
//  Copyright © 2019 Arthur Masson. All rights reserved.
//

#include "GLSSkinnedMesh.hpp"

namespace GLS {

    std::shared_ptr<SkinnedMesh> SkinnedMesh::loadFromAiMesh(aiMesh *mesh, std::shared_ptr<Node> sceneRootBone, std::shared_ptr<Node> rootBone, bool generateBuffers) {
        std::shared_ptr<SkinnedMesh> nMesh = std::make_shared<SkinnedMesh>();
        nMesh->_rootBone.node = rootBone;
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
            nMesh->verticesRef().push_back(v);
        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                nMesh->indicesRef().push_back(face.mIndices[j]);
            }
        }
        for (unsigned int i = 0; i < mesh->mNumBones; i++) {
            std::string boneName(mesh->mBones[i]->mName.data);
            Bone nBone;
            if (rootBone->name() == boneName)
                nBone.node = sceneRootBone;
            else
                nBone.node = sceneRootBone->childNodeNamed(boneName, true);
            nBone.offset = aiToGlm(mesh->mBones[i]->mOffsetMatrix);
            nMesh->_bones.push_back(nBone);
            for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
                aiVertexWeight weight = mesh->mBones[i]->mWeights[j];
                if (!nMesh->verticesRef()[weight.mVertexId].addWeight(static_cast<int>(nMesh->_bones.size()) - 1, weight.mWeight))
                    std::cout << "join overflow" << std::endl;
            }
        }
        if (generateBuffers)
            nMesh->generateBuffers();
        return nMesh;
    }

}
