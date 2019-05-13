//
//  GLSNode_loader.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSNode.hpp"

namespace GLS {

    static void _processLoadNode(Node *n, aiNode *node, const aiScene *scene, std::vector<std::shared_ptr<Material> >& materials) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            std::shared_ptr<Mesh> nMesh = Mesh::loadFromAiMesh(mesh);
            nMesh->setMaterial(materials[mesh->mMaterialIndex]);
            n->addRenderable(nMesh);
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            _processLoadNode(n, node->mChildren[i], scene, materials);
        }
    }

    void Node::loadMeshFromFile(std::string path) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            return;
            // throw LoadFileException();
        }
        std::string directory = path.substr(0, path.find_last_of('/'));
        std::vector<std::shared_ptr<Material> > materials;
        for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
            materials.push_back(Material::loadFromAiMaterial(scene->mMaterials[i], directory));
        }
        materials.push_back(std::make_shared<Material>());
        _processLoadNode(this, scene->mRootNode, scene, materials);
    }

}
