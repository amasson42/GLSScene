//
//  GLSMesh_loader.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSMesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace GLS {

    // TODO !
    std::shared_ptr<Mesh> Mesh::objModel(std::string path, int options, bool generateBuffers) /*throw(LoadMeshException)*/ {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

        (void)path;
        (void)options;

        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            return nullptr;
            // throw LoadMeshException(filename);
        }

        if (generateBuffers)
            mesh->generateBuffers();
        return mesh;
    }

}
