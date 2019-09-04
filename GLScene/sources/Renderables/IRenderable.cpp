//
//  GLSIRenderable.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSIRenderable.hpp"

namespace GLS {

    IRenderable::IRenderable() :
    _castShadow(true)
    {}

    IRenderable::IRenderable(const IRenderable& copy) :
    _castShadow(copy._castShadow)
    {}

    IRenderable::~IRenderable()
    {}

    IRenderable& IRenderable::operator=(const IRenderable& copy) {
        _castShadow = copy._castShadow;
        return *this;
    }

    std::pair<glm::vec3, glm::vec3> IRenderable::getBounds(glm::mat4 transform) const {
        return std::make_pair<glm::vec3, glm::vec3>(glm::vec3(0), glm::vec3(0));
    }

    void IRenderable::setCastShadow(bool v) {
        _castShadow = v;
    }

    bool IRenderable::castShadow() const {
        return _castShadow;
    }

    void IRenderable::renderInDepthContext(Scene& scene, const RenderUniforms& uniforms) {
        (void)scene;
        (void)uniforms;
    }

    void IRenderable::postRenderInContext(Scene& scene, const RenderUniforms& uniforms, float priority) {
        (void)scene;
        (void)uniforms;
        (void)priority;
    }

    void IRenderable::sendToFlux(std::ostream& flux, std::string linePrefix) const {
        flux << linePrefix << "[IRenderable]" << std::endl;
    }

    std::string IRenderable::shaderUniformsVertex() {
        return "";
    }

    std::string IRenderable::shaderUniformsGeometry() {
        return "";
    }

    std::string IRenderable::shaderUniformsFragment() {
        return "";
    }

}
