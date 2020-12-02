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
    _transform(),
    _parent(), _childs(),
    _active(true),
    _camera(nullptr), _renderables(), _animatables()
    {

    }

    Node::Node(const Node& copy) :
    _name(copy._name),
    _transform(copy._transform),
    _parent(), _childs(),
    _active(copy._active),
    _camera(copy._camera), _renderables(copy._renderables), _animatables(copy._animatables)
    {
        for (size_t i = 0; i < copy._childs.size(); i++)
            addChildNode(std::make_shared<Node>(*copy._childs[i]));
    }

    Node::~Node() {
        
    }

    Node& Node::operator=(const Node& copy) {
        _name = copy._name;
        _transform = copy._transform;
        _active = copy._active;
        _camera = copy._camera;
        _renderables = copy._renderables;
        _animatables = copy._animatables;
        for (size_t i = 0; i < copy._childs.size(); i++)
            addChildNode(std::make_shared<Node>(*copy._childs[i]));
        return *this;
    }

}
