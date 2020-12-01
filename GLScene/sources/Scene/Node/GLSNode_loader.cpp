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

namespace GLS {

    static void _processLoadNode(std::shared_ptr<Node> n,
        aiNode *node,
        const aiScene *scene,
        std::vector<std::shared_ptr<Material> >& materials,
        std::vector<std::pair<std::shared_ptr<Node>, aiMesh*>>& nodeMeshes) {
        {
            n->setName(std::string(node->mName.C_Str()));
            n->setTransform(Transform::loadFromAiMatrix(node->mTransformation));
            n->transform().updateMatrix();
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            std::shared_ptr<Node> newChild = std::make_shared<Node>();
            n->addChildNode(newChild);
            _processLoadNode(newChild, node->mChildren[i], scene, materials, nodeMeshes);
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

    std::shared_ptr<Node> Node::loadFromFile(std::string path) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
        try {
            return loadFromAiScene(scene, path.substr(0, path.find_last_of('/')));
        } catch (FileLoadingException& e) {
            throw FileLoadingException("can't load scene from file " + path);
        }
    }

    std::shared_ptr<Node> Node::loadFromAiScene(const aiScene *scene, std::string directory) {
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw FileLoadingException("can't load scene from aiScene");
        }

        std::shared_ptr<Node> node = std::make_shared<Node>();

        std::cout << "scene "
            << "mNumAnimations " << scene->mNumAnimations << " | "
            << "mNumCameras " << scene->mNumCameras << " | "
            << "mNumLights " << scene->mNumLights << " | "
            << "mNumMaterials " << scene->mNumMaterials << " | "
            << "mNumMeshes " << scene->mNumMeshes << " | "
            << "mNumTextures " << scene->mNumTextures << " | "
            << std::endl;
        std::vector<std::shared_ptr<Material> > materials;
        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
            materials.push_back(Material::loadFromAiMaterial(scene->mMaterials[i], directory, static_cast<aiTexture **>(scene->mTextures)));
        materials.push_back(std::make_shared<Material>());

        std::vector<std::pair<std::shared_ptr<Node>, aiMesh*>> nodeMeshes;
        _processLoadNode(node, scene->mRootNode, scene, materials, nodeMeshes);

        if (scene->HasAnimations()) {
            node->setSkeleton(Skeleton::loadFromAiSceneAnimations(scene, node.get()));
        }

        for (size_t i = 0; i < nodeMeshes.size(); i++) {
            std::pair<std::shared_ptr<Node>, aiMesh*> nodeMesh = nodeMeshes[i];
            if (nodeMesh.second->HasBones()) {
                std::shared_ptr<SkinnedMesh> nSkinnedMesh = SkinnedMesh::loadFromAiMesh(nodeMesh.second, node, nodeMesh.first);
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
