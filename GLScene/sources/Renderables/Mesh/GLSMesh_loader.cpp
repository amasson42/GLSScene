//
//  GLSMesh_load.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"

namespace GLS {

    std::shared_ptr<Mesh> Mesh::loadFromAiMesh(aiMesh *mesh, bool generateBuffers) {
        std::shared_ptr<Mesh> nMesh = std::make_shared<Mesh>();
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex v;
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
                v = Vertex(v.position, v.normal, v.uv);
            nMesh->verticesRef().push_back(v);
        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                nMesh->indicesRef().push_back(face.mIndices[j]);
            }
        }
        if (!mesh->HasNormals()) {
            nMesh->calculNormals();
        }
        if (generateBuffers)
            nMesh->generateBuffers();
        return nMesh;
    }

    std::shared_ptr<Mesh> Mesh::objModel(std::string path, bool generateBuffers) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw FileLoadingException("can't open file " + path);
        }

        std::shared_ptr<Mesh> mesh;
        if (scene->HasMeshes())
            mesh = Mesh::loadFromAiMesh(scene->mMeshes[0]);

        if (generateBuffers)
            mesh->generateBuffers();
        return mesh;
    }

}
