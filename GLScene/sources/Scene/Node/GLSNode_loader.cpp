//
//  GLSNode_loader.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSNode.hpp"

namespace GLS {

    static void _processLoadNode(std::shared_ptr<Node> n, aiNode *node, const aiScene *scene, std::shared_ptr<Node> rootNode, std::vector<std::shared_ptr<Material> >& materials) {
        if (n == nullptr) {
            std::cout << "send nullptr" << std::endl;
        }
        {
            n->setName(std::string(node->mName.C_Str()));
            aiVector3D scale;
            aiQuaternion rotation;
            aiVector3D translate;
            node->mTransformation.Decompose(scale, rotation, translate);
            n->transform().setPosition(glm::vec3(translate.x, translate.y, translate.z));
            n->transform().setRotation(glm::quat(rotation.x, rotation.y, rotation.z, rotation.z));
            n->transform().setScale(glm::vec3(scale.x, scale.y, scale.z));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            std::shared_ptr<Node> newChild = std::make_shared<Node>();
            n->addChildNode(newChild);
            _processLoadNode(newChild, node->mChildren[i], scene, rootNode, materials);
        }
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            if (mesh->HasBones()) {
                std::shared_ptr<SkinnedMesh> nSkinnedMesh = SkinnedMesh::loadFromAiMesh(mesh, rootNode, n);
                nSkinnedMesh->setMaterial(materials[mesh->mMaterialIndex]);
                n->addRenderable(nSkinnedMesh);
            } else {
                std::shared_ptr<Mesh> nMesh = Mesh::loadFromAiMesh(mesh);
                nMesh->setMaterial(materials[mesh->mMaterialIndex]);
                n->addRenderable(nMesh);
            }
        }
    }

    void Node::loadFromFile(std::string path) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "invalid scene " << path << std::endl;
            return;
            // TODO: throw LoadFileException();
        }
        std::string directory = path.substr(0, path.find_last_of('/'));
        std::vector<std::shared_ptr<Material> > materials;
        for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
            materials.push_back(Material::loadFromAiMaterial(scene->mMaterials[i], directory));
        }
        materials.push_back(std::make_shared<Material>());
        _processLoadNode(shared_from_this(), scene->mRootNode, scene, shared_from_this(), materials);

        std::cout << "scene: " << scene->mNumAnimations << std::endl;
    }

}
