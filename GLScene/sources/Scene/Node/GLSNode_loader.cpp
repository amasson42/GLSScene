//
//  GLSNode_load.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSNode.hpp"
#include "GLSMesh.hpp"
#include "GLSSkinnedMesh.hpp"
#include "GLSSkeleton.hpp"

namespace GLS {

    std::shared_ptr<Assimp::Importer> Node::_assimpImporter = nullptr;

    static void _processLoadNode(std::shared_ptr<Node> n,
        aiNode *node,
        const aiScene *scene,
        std::vector<std::pair<std::shared_ptr<Node>, aiMesh*>>& nodeMeshes) {
        {
            n->setName(std::string(node->mName.C_Str()));
            n->setTransform(Transform::loadFromAiMatrix(node->mTransformation));
            n->transform().updateMatrix();
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            std::shared_ptr<Node> newChild = std::make_shared<Node>();
            n->addChildNode(newChild);
            _processLoadNode(newChild, node->mChildren[i], scene, nodeMeshes);
        }
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            std::pair<std::shared_ptr<Node>, aiMesh*> pair;
            pair.first = n;
            pair.second = mesh;
            nodeMeshes.push_back(pair);
            // FIXME: Why can't we make it in one line ?
            // nodeMeshes.push_back(std::make_pair<std::shared_ptr<Node>, aiMesh*>(n, mesh));
        }
    }

    std::shared_ptr<Node> Node::loadFromFile(std::string path) {
        const aiScene *scene = _assimpImporter->ReadFile(path,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
        try {
            return loadFromAiScene(scene, path.substr(0, path.find_last_of('/')));
        } catch (FileLoadingException& e) {
            throw FileLoadingException("can't load scene from file " + path);
        }
    }

    std::shared_ptr<Node> Node::loadFromAiScene(const aiScene *scene, std::string directory) {
        if (!scene) {
            throw FileLoadingException("No scene input");
        }
        std::cout << "scene "
                << "validated: " << (scene->mFlags & AI_SCENE_FLAGS_VALIDATED) << " | "
                << "incomplete: " << (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) << " | "
                << "mNumAnimations " << scene->mNumAnimations << " | "
                << "mNumCameras " << scene->mNumCameras << " | " // TODO: Add them
                << "mNumLights " << scene->mNumLights << " | " // TODO: Add them
                << "mNumMaterials " << scene->mNumMaterials << " | "
                << "mNumMeshes " << scene->mNumMeshes << " | "
                << "mNumTextures " << scene->mNumTextures << " | "
                << std::endl;
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw FileLoadingException("can't load scene from aiScene");
        }

        std::shared_ptr<Node> node = std::make_shared<Node>();

        std::vector<std::shared_ptr<Material> > materials;
        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
            materials.push_back(Material::loadFromAiMaterial(scene->mMaterials[i], directory, static_cast<aiTexture **>(scene->mTextures)));
        materials.push_back(std::make_shared<Material>());

        std::vector<std::pair<std::shared_ptr<Node>, aiMesh*>> nodeMeshes;
        _processLoadNode(node, scene->mRootNode, scene, nodeMeshes);

        std::shared_ptr<Skeleton> skeleton = nullptr;
        if (scene->HasAnimations()) {
            skeleton = Skeleton::loadFromAiSceneAnimations(scene, node);
            node->addAnimatable(skeleton);
        }

        for (size_t i = 0; i < nodeMeshes.size(); i++) {
            std::pair<std::shared_ptr<Node>, aiMesh*>& nodeMesh(nodeMeshes[i]);
            if (nodeMesh.second->HasBones()) {
                std::shared_ptr<SkinnedMesh> nSkinnedMesh = SkinnedMesh::loadFromAiMesh(nodeMesh.second, skeleton, node /* nodeMesh.first */);
                nSkinnedMesh->setMaterial(materials[nodeMesh.second->mMaterialIndex]);
                nodeMesh.first->addRenderable(nSkinnedMesh);
            } else {
                std::shared_ptr<Mesh> nMesh = Mesh::loadFromAiMesh(nodeMesh.second);
                nMesh->setMaterial(materials[nodeMesh.second->mMaterialIndex]);
                nodeMesh.first->addRenderable(nMesh);
            }
        }

        return node;
    }

}
