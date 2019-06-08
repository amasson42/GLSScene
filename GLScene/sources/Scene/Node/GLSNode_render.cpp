//
//  GLSNode_render.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSNode.hpp"

namespace GLS {

    void Node::renderInContext(Scene& scene, RenderUniforms uniforms) {
        if (!_active)
            return;
        _transform.updateMatrix();
        uniforms.model = uniforms.model * getTransformMatrix();
        for (size_t i = 0; i < _childs.size(); i++) {
            _childs[i]->renderInContext(scene, uniforms);
        }
        for (size_t i = 0; i < _renderables.size(); i++) {
            _renderables[i]->renderInContext(scene, uniforms);
        }
    }
    
    void Node::renderInDepthContext(Scene& scene, RenderUniforms uniforms) {
        if (!_active)
            return;
        _transform.updateMatrix();
        uniforms.model = uniforms.model * getTransformMatrix();
        for (size_t i = 0; i < _childs.size(); i++) {
            _childs[i]->renderInDepthContext(scene, uniforms);
        }
        for (size_t i = 0; i < _renderables.size(); i++) {
            _renderables[i]->renderInDepthContext(scene, uniforms);
        }
    }

}
