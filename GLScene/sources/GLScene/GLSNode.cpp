//
//  GLSNode.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSNode.hpp"

namespace GLS {
    
    Node::Node() :
    _name("empty_node"),
    _position(0, 0, 0), _rotation(0, 0, 0), _scale(1, 1, 1),
    _transformUpdated(false),
    _parent(nullptr), _childs(),
    _camera(nullptr), _mesh(nullptr)
    {
        updateTransformMatrix();
    }
    
    Node::Node(const Node& copy) :
    _name(copy._name),
    _position(copy._position), _rotation(copy._rotation), _scale(copy._scale),
    _transformUpdated(false),
    _parent(nullptr), _childs(),
    _camera(copy._camera), _mesh(copy._mesh)
    {
        if (copy._transformUpdated)
            updateTransformMatrix();
        for (size_t i = 0; i < copy._childs.size(); i++)
            addChildNode(std::make_shared<Node>(*copy._childs[i]));
    }
    
    Node::~Node() {
        
    }
    
    Node& Node::operator=(const Node& copy) {
        _name = copy._name;
        _position = copy._position;
        _rotation = copy._rotation;
        _scale = copy._scale;
        _transformUpdated = copy._transformUpdated;
        _parent = nullptr;
        _camera = copy._camera;
        _mesh = copy._mesh;
        if (copy._transformUpdated)
            updateTransformMatrix();
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
    
    Vector Node::position() const {
        return _position;
    }
    
    void Node::setPosition(Vector position) {
        _position = position;
        _transformUpdated = false;
    }
    
    Vector Node::rotation() const {
        return _rotation;
    }
    
    void Node::setRotation(Vector rotation) {
        _rotation = rotation;
        _transformUpdated = false;
    }
    
    Vector Node::scale() const {
        return _scale;
    }
    
    void Node::setScale(Vector scale) {
        _scale = scale;
        _transformUpdated = false;
    }
    
    static Matrix4x4 calculTransformMatrix(Vector position, Vector rotation, Vector scale) {
        return
        Matrix4x4::translation(position)
        * Matrix4x4::rotx(rotation.x)
        * Matrix4x4::roty(rotation.y)
        * Matrix4x4::rotz(rotation.z)
        * Matrix4x4::scale(scale);
    }
    
    Matrix4x4 Node::getTransformMatrix() {
        if (!_transformUpdated)
            updateTransformMatrix();
        return _transform;
    }
    
    Matrix4x4 Node::getTransformMatrix() const {
        if (_transformUpdated)
            return _transform;
        else
            return calculTransformMatrix(_position, _rotation, _scale);
    }
    
    Matrix4x4 Node::getWorldTransformMatrix() {
        if (_parent)
            return _parent->getTransformMatrix() * getTransformMatrix();
        else
            return getTransformMatrix();
    }
    
    Matrix4x4 Node::getWorldTransformMatrix() const {
        if (_parent)
            return _parent->getTransformMatrix() * getTransformMatrix();
        else
            return getTransformMatrix();
    }
    
    void Node::updateTransformMatrix() {
        if (!_transformUpdated)
            _transform = calculTransformMatrix(_position, _rotation, _scale);
        _transformUpdated = true;
    }
    
    
    // Hierarchy
    
    std::vector<std::shared_ptr<Node> >& Node::childNodes() {
        return _childs;
    }
    
    void Node::addChildNode(std::shared_ptr<Node> node) {
        _childs.push_back(node);
    }
    
    void Node::removeChildNode(Node *node) {
        std::vector<std::shared_ptr<Node> >::iterator it;
        for (it = _childs.begin(); it != _childs.end(); ++it)
            if (&(**it) == node)
                return static_cast<void>(_childs.erase(it));
    }
    
    void Node::removeFromParent() {
        if (_parent) {
            _parent->removeChildNode(this);
        }
    }
    
    
    // Node functionalities
    
    const std::shared_ptr<const Mesh> Node::mesh() const {
        return _mesh;
    }
    
    std::shared_ptr<Mesh> Node::mesh() {
        return _mesh;
    }
    
    void Node::setMesh(std::shared_ptr<Mesh> mesh) {
        _mesh = mesh;
    }
    
    std::pair<Vector, Vector> Node::getBounds() const {
        // TODO: this shit
        return _mesh->getBounds();
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
    
    // SOON: Lights
    
    void Node::renderInContext(ShaderProgram& program, Matrix4x4 view) {
        updateTransformMatrix();
        for (size_t i = 0; i < _childs.size(); i++) {
            _childs[i]->renderInContext(program, view * _transform);
        }
        if (_mesh)
            _mesh->renderInContext(program, view, _transform);
    }
    
}
