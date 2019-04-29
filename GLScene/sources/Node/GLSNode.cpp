//
//  GLSNode.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSNode.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace GLS {
    
    Node::Node() :
    _name("empty_node"),
    _transform(),
    _parent(nullptr), _childs(),
    _camera(nullptr), _renderables()
    {

    }
    
    Node::Node(const Node& copy) :
    _name(copy._name),
    _transform(copy._transform),
    _parent(nullptr), _childs(),
    _camera(copy._camera), _renderables(copy._renderables)
    {
        for (size_t i = 0; i < copy._childs.size(); i++)
            addChildNode(std::make_shared<Node>(*copy._childs[i]));
    }
    
    Node::~Node() {
        
    }
    
    Node& Node::operator=(const Node& copy) {
        _name = copy._name;
        _transform = copy._transform;
        _parent = nullptr;
        _camera = copy._camera;
        _renderables = copy._renderables;
        for (size_t i = 0; i < copy._childs.size(); i++)
            addChildNode(std::make_shared<Node>(*copy._childs[i]));
        return *this;
    }
    
    std::string Node::name() const {
        return _name;
    }
    
    void Node::setName(std::string name) {
        _name = name;
    }
    
    
    // Transformation
    
    Transform& Node::transform() {
        return _transform;
    }

    const Transform& Node::transform() const {
        return _transform;
    }

    const glm::mat4& Node::getTransformMatrix() {
        return _transform.matrix();
    }

    const glm::mat4 Node::getTransformMatrix() const {
        return _transform.matrix();
    }

    const glm::mat4 Node::getWorldTransformMatrix() {
        if (_parent)
            return _parent->getWorldTransformMatrix() * getTransformMatrix();
        else
            return getTransformMatrix();
    }
    
    const glm::mat4 Node::getWorldTransformMatrix() const {
        if (_parent)
            return _parent->getWorldTransformMatrix() * getTransformMatrix();
        else
            return getTransformMatrix();
    }

    // Hierarchy
    
    const std::vector<std::shared_ptr<Node> >& Node::childNodes() {
        return _childs;
    }

    std::shared_ptr<Node> Node::childNodeAt(size_t i) const {
        if (i < _childs.size())
            return _childs[i];
        return nullptr;
    }
    
    void Node::addChildNode(std::shared_ptr<Node> node) {
        if (node->hasParentNode(this) || this->hasParentNode(node.get()))
            return;
        _childs.push_back(node);
    }
    
    void Node::removeChildNode(Node *node) {
        std::vector<std::shared_ptr<Node> >::iterator it;
        for (it = _childs.begin(); it != _childs.end(); ++it)
            if (&(**it) == node)
                return static_cast<void>(_childs.erase(it));
    }
    
    bool Node::hasParentNode(Node* node) const {
        if (_parent == nullptr) {
            return false;
        } else if (_parent == node) {
            return true;
        } else {
            return _parent->hasParentNode(node);
        }
    }

    void Node::removeFromParent() {
        if (_parent) {
            _parent->removeChildNode(this);
        }
    }
    
    
    // Node functionalities
    
    const std::vector<std::shared_ptr<IRenderable> >& Node::renderables() const {
        return _renderables;
    }

    void Node::addRenderable(std::shared_ptr<IRenderable> renderable) {
        _renderables.push_back(renderable);
    }

    void Node::removeRenderableIndex(size_t i) {
        if (i < _renderables.size() - 1) {
            _renderables.erase(_renderables.begin() + i);
        } else if (i == _renderables.size() - 1) {
            _renderables.pop_back();
        }
    }
    
    static std::pair<glm::vec3, glm::vec3> mergeBounds(std::pair<glm::vec3, glm::vec3> b1, std::pair<glm::vec3, glm::vec3> b2) {
        std::pair<glm::vec3, glm::vec3> bounds;
        bounds.first = glm::vec3(std::min(b1.first.x, b2.first.x),
                                 std::min(b1.first.y, b2.first.y),
                                 std::min(b1.first.z, b2.first.z));
        bounds.second = glm::vec3(std::max(b1.second.x, b2.second.x),
                                  std::max(b1.second.y, b2.second.y),
                                  std::max(b1.second.z, b2.second.z));
        return bounds;
    }

    std::pair<glm::vec3, glm::vec3> Node::getBounds() const {
        glm::mat4 modelMatrix = getWorldTransformMatrix();
        std::pair<glm::vec3, glm::vec3> bounds;
        for (size_t i = 0; i < _renderables.size(); i++) {
            bounds = mergeBounds(bounds, _renderables[i]->getBounds(modelMatrix));
        }
        for (size_t i = 0; i < _childs.size(); i++) {
            bounds = mergeBounds(bounds, _childs[i]->getBounds());
        }
        return bounds;
    }
    
    static std::shared_ptr<Mesh> _processLoadMesh(Node *n, aiMesh *mesh, const aiScene *scene) {
        (void)n;
        (void)scene;
        std::shared_ptr<Mesh> nMesh = std::make_shared<Mesh>();

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex v;
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
                v = Vertex(v.position, v.normal, v.uv);
            nMesh->verticesRef().push_back(v);
        }
        if (!mesh->HasNormals()) {
            nMesh->calculNormals();
        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                nMesh->indicesRef().push_back(face.mIndices[j]);
            }
        }
        // if (mesh->mMaterialIndex[0] >= 0) {
        //     aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        //     // std::vector<std::shared_ptr<Texture> > diffuseMaps =
        //     //     _loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        // }
        nMesh->generateBuffers();

        return nMesh;
    }

    static void _processLoadNode(Node *n, aiNode *node, const aiScene *scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            n->addRenderable(_processLoadMesh(n, mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            _processLoadNode(n, node->mChildren[i], scene);
        }
    }

    // static std::vector<std::shared_ptr<Texture> > _loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {

    // }

    void Node::loadMeshFromFile(std::string path) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            return;
        std::string directory = path.substr(0, path.find_last_of('/'));
        _processLoadNode(this, scene->mRootNode, scene);
    }

    const std::shared_ptr<const Camera> Node::camera() const {
        return _camera;
    }
    
    std::shared_ptr<Camera> Node::camera() {
        return _camera;
    }
    
    void Node::setCamera(std::shared_ptr<Camera> camera) {
        _camera = camera;
    }
    
    const std::shared_ptr<const Light> Node::light() const {
        return _light;
    }

    std::shared_ptr<Light> Node::light() {
        return _light;
    }

    void Node::setLight(std::shared_ptr<Light> light) {
        _light = light;
    }

    void Node::_getAllLights(std::vector<Light>& container, glm::mat4 parentMatrix) {
        glm::mat4 mat = parentMatrix * getTransformMatrix();
        if (_light != nullptr)
            container.push_back(_light->transformedBy(mat));
        for (size_t i = 0; i < _childs.size(); i++) {
            _childs[i]->_getAllLights(container, mat);
        }
    }
    
    void Node::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
        _transform.updateMatrix();
        RenderUniforms nodeUniforms(uniforms);
        nodeUniforms.model = uniforms.model * getTransformMatrix();
        for (size_t i = 0; i < _childs.size(); i++) {
            _childs[i]->renderInContext(scene, nodeUniforms);
        }
        for (size_t i = 0; i < _renderables.size(); i++) {
            _renderables[i]->renderInContext(scene, nodeUniforms);
        }
    }
    
}
