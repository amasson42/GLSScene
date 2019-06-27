//
//  GLSNode_hierarchy.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSNode.hpp"

namespace GLS {

    const std::vector<std::shared_ptr<Node> >& Node::childNodes() const {
        return _childs;
    }

    std::shared_ptr<Node> Node::childNodeAt(size_t i) const {
        if (i < _childs.size())
            return _childs[i];
        return nullptr;
    }

    std::shared_ptr<Node> Node::childNodeNamed(std::string name, bool recursively) const {
        for (size_t i = 0; i < _childs.size(); i++) {
            if (_childs[i]->_name == name) {
                return _childs[i];
            }
            if (recursively) {
                std::shared_ptr<Node> n = _childs[i]->childNodeNamed(name, recursively);
                if (n != nullptr)
                    return n;
            }
        }
        return nullptr;
    }

    void Node::addChildNode(std::shared_ptr<Node> node) {
        node->removeFromParent();
        _childs.push_back(node);
        node->_parent = shared_from_this();
    }

    void Node::removeChildNode(std::shared_ptr<Node> node) {
        removeChildNode(node.get());
    }

    void Node::removeChildNode(Node* node) {
        std::vector<std::shared_ptr<Node> >::iterator it;
        for (it = _childs.begin(); it != _childs.end(); ++it)
            if (it->get() == node) {
                (*it)->_parent = std::weak_ptr<Node>();
                _childs.erase(it);
                return;
            }
    }

    std::shared_ptr<Node> Node::getParentNode() const {
        if (_parent.expired())
            return nullptr;
        return _parent.lock();
    }

    bool Node::hasParentNode(std::shared_ptr<Node> node) const {
        if (_parent.expired())
            return false;
        std::shared_ptr<Node> parent = _parent.lock();
        if (parent.get() == node.get())
            return true;
        return parent->hasParentNode(node);
    }

    void Node::removeFromParent() {
        if (!_parent.expired()) {
            _parent.lock()->removeChildNode(this);
        }
    }

}
