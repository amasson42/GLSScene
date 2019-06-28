//
//  GLSSkinnedMesh_loader.cpp
//  GLScene
//
//  Created by Arthur Masson on 26/06/2019.
//  Copyright © 2019 Arthur Masson. All rights reserved.
//

#include "GLSSkinnedMesh.hpp"

namespace GLS {

    static glm::mat4 _assimpToGlmMat4(aiMatrix4x4 mat) {
        aiVector3D scale;
        aiQuaternion rotation;
        aiVector3D translate;
        mat.Decompose(scale, rotation, translate);
        Transform t;
        t.setPosition(glm::vec3(translate.x, translate.y, translate.z));
        t.setRotation(glm::quat(rotation.x, rotation.y, rotation.z, rotation.z));
        t.setScale(glm::vec3(scale.x, scale.y, scale.z));
        return t.matrix();
    }

    std::shared_ptr<SkinnedMesh> SkinnedMesh::loadFromAiMesh(aiMesh *mesh, std::shared_ptr<Node> sceneRootBone, std::shared_ptr<Node> rootBone, bool generateBuffers) {
        std::shared_ptr<SkinnedMesh> nMesh = std::make_shared<SkinnedMesh>();
        nMesh->_rootBone.node = rootBone;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            SkinnedVertex v;
            v.position.x = mesh->mVertices[i].x;
            v.position.y = mesh->mVertices[i].y;
            v.position.z = mesh->mVertices[i].z;
            if (mesh->HasNormals()) {
                v.normal.x = mesh->mNormals[i].x;
                v.normal.y = mesh->mNormals[i].y;
                v.normal.z = mesh->mNormals[i].z;
            }
            if (mesh->mTextureCoords[0]) {
                v.uv.x = mesh->mTextureCoords[0][i].x;
                v.uv.y = mesh->mTextureCoords[0][i].y;
            }
            if (mesh->HasTangentsAndBitangents()) {
                v.tangent.x = mesh->mTangents[i].x;
                v.tangent.y = mesh->mTangents[i].y;
                v.tangent.z = mesh->mTangents[i].z;
                v.bitangent.x = mesh->mBitangents[i].x;
                v.bitangent.y = mesh->mBitangents[i].y;
                v.bitangent.z = mesh->mBitangents[i].z;
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
            nBone.offset = _assimpToGlmMat4(mesh->mBones[i]->mOffsetMatrix);
            nMesh->_bones.push_back(nBone);
            for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
                aiVertexWeight weight = mesh->mBones[i]->mWeights[j];
                if (!nMesh->verticesRef()[weight.mVertexId].addWeight(nMesh->_bones.size() - 1, weight.mWeight))
                    std::cout << "join overflow" << std::endl;
            }
        }
        if (generateBuffers)
            nMesh->generateBuffers();
        return nMesh;
    }

}
