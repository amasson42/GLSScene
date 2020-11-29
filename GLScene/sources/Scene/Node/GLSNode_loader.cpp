//
//  GLSNode_loader.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSNode.hpp"
#include "GLSMesh.hpp"
#include "GLSSkinnedMesh.hpp"

namespace GLS {

    static void _processLoadNode(std::shared_ptr<Node> n,
        aiNode *node,
        const aiScene *scene,
        std::shared_ptr<Node> rootNode,
        std::vector<std::shared_ptr<Material> >& materials,
        std::vector<std::pair<std::shared_ptr<Node>, aiMesh*>>& nodeMeshes) {
        {
            n->setName(std::string(node->mName.C_Str()));
            aiVector3D scale;
            aiQuaternion rotation;
            aiVector3D translate;
            node->mTransformation.Decompose(scale, rotation, translate);
            n->transform().setPosition(glm::vec3(translate.x, translate.y, translate.z));
            n->transform().setRotation(glm::quat(rotation.x, rotation.y, rotation.z, rotation.w));
            n->transform().setScale(glm::vec3(scale.x, scale.y, scale.z));
            n->transform().updateMatrix();
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            std::shared_ptr<Node> newChild = std::make_shared<Node>();
            n->addChildNode(newChild);
            _processLoadNode(newChild, node->mChildren[i], scene, rootNode, materials, nodeMeshes);
        }
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            std::pair<std::shared_ptr<Node>, aiMesh*> pair;
            pair.first = n;
            pair.second = mesh;
            nodeMeshes.push_back(pair);
            // FIXME: Ok but wtf ?
            // nodeMeshes.push_back(std::make_pair<std::shared_ptr<Node>, aiMesh*>(n, mesh));
        }
    }

    void Node::loadFromFile(std::string path) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw FileLoadingException("can't load scene from " + path);
        }

        std::string directory = path.substr(0, path.find_last_of('/'));
        std::vector<std::shared_ptr<Material> > materials;
        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
            materials.push_back(Material::loadFromAiMaterial(scene->mMaterials[i], directory));
        materials.push_back(std::make_shared<Material>());

        std::vector<std::pair<std::shared_ptr<Node>, aiMesh*>> nodeMeshes;
        _processLoadNode(shared_from_this(), scene->mRootNode, scene, shared_from_this(), materials, nodeMeshes);

        if (scene->HasAnimations()) {
            setSkeleton(Skeleton::loadFromAiSceneAnimations(scene, this));
        }

        for (int i = 0; i < nodeMeshes.size(); i++) {
            std::pair<std::shared_ptr<Node>, aiMesh*> nodeMesh = nodeMeshes[i];
            if (nodeMesh.second->HasBones()) {
                std::shared_ptr<SkinnedMesh> nSkinnedMesh = SkinnedMesh::loadFromAiMesh(nodeMesh.second, shared_from_this(), nodeMesh.first);
                nSkinnedMesh->setMaterial(materials[nodeMesh.second->mMaterialIndex]);
                nodeMesh.first->addRenderable(nSkinnedMesh);
            } else {
                std::shared_ptr<Mesh> nMesh = Mesh::loadFromAiMesh(nodeMesh.second);
                nMesh->setMaterial(materials[nodeMesh.second->mMaterialIndex]);
                nodeMesh.first->addRenderable(nMesh);
            }
        }

    }

}
