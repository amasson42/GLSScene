//
//  GLSScene_render.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSScene.hpp"

namespace GLS {

    void Scene::_renderInLightCasterContext(LightCaster& caster) {
        caster.depth_map->bind();
        glViewport(0, 0, caster.depth_map->width(), caster.depth_map->height());
        glClear(GL_DEPTH_BUFFER_BIT);
        RenderUniforms uniforms;
        uniforms.view = glm::inverse(caster.light._view);
        uniforms.camera_position = glm::vec3(caster.light._view * glm::vec4(0, 0, 0, 1));
        uniforms.projection = caster.light._projection;
        _rootNode->renderInDepthContext(*this, uniforms);
        caster.depth_map->unbind();
    }

    void Scene::_calculLights() {
        _rootNode->getAllLights(_frameLights, glm::mat4(1));
        for (size_t i = 0; i < _frameLights.size() && _frameLightCasters.size() < 4; i++) {
            if (_frameLights[i].cast_shadow && Light::lightTypeCanCastShadow(_frameLights[i].type)) {
                LightCaster caster(_frameLights[i]);
                _renderInLightCasterContext(caster);
                _frameLightCasters.push_back(caster);
                _frameLights[i]._caster_index = _frameLightCasters.size() - 1;
            }
        }
    }

    RenderUniforms Scene::makeUniforms() const {
        RenderUniforms uniforms;
        if (!_cameraNode.expired()) {
            std::shared_ptr<Node> camera = _cameraNode.lock();
            glm::mat4 camMatrix = camera->getWorldTransformMatrix();
            uniforms.view = glm::inverse(camMatrix);
            uniforms.camera_position = glm::vec3(camMatrix * glm::vec4(0, 0, 0, 1));
            if (camera->camera() != nullptr)
                uniforms.projection = camera->camera()->projectionMatrix();
        }
        return uniforms;
    }

    void Scene::sendLightsValueToShader(std::shared_ptr<ShaderProgram> program) {
        program->use();
        for (size_t i = 0; i < _frameLights.size() && i < 16; i++) {
            _frameLights[i].sendUniformsToShaderProgram(program, static_cast<int>(i));
        }
        glUniform1i(program->getLocation("lights_count"), static_cast<int>(_frameLights.size()));
        for (size_t i = 0; i < _frameLightCasters.size() && i < 4; i++) {
            _frameLightCasters[i].sendUniformsToShaderProgram(program, static_cast<int>(i));
        }
    }

    typedef std::pair<IRenderable*, RenderUniforms> RenderAndUniforms;

    void Scene::renderInContext(std::shared_ptr<Framebuffer> framebuffer) {
        
        _frameLights.clear();
        _frameLightCasters.clear();
        _postRenderables.clear();
        _calculLights();

        if (framebuffer == 0) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        } else {
            framebuffer->bind();
        }

        glViewport(0, 0, _size.x, _size.y);
        glClearColor(_background.x, _background.y, _background.z, _background.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        RenderUniforms uniforms = makeUniforms();
        
        sendLightsValueToShader(ShaderProgram::standardShaderProgramMesh());
        sendLightsValueToShader(ShaderProgram::standardShaderProgramInstancedMesh());
        sendLightsValueToShader(ShaderProgram::standardShaderProgramSkinnedMesh());
        sendLightsValueToShader(ShaderProgram::standardShaderProgramVoxelChunk());
        
        _rootNode->renderInContext(*this, uniforms);
        
        std::multimap<float, RenderAndUniforms>::iterator itr;
        for (itr = _postRenderables.begin(); itr != _postRenderables.end(); ++itr) {
            IRenderable *renderable = itr->second.first;
            RenderUniforms uniforms = itr->second.second;
            float priority = itr->first;
            renderable->postRenderInContext(*this, uniforms, priority);
        }

        if (_skybox != nullptr) {
            _skybox->renderInContext(*this, uniforms);
        }

    }
    
    void Scene::subscribeToPostRenderable(IRenderable* renderable, const RenderUniforms& uniforms, float priority) {
        RenderAndUniforms paired(renderable, uniforms);
        _postRenderables.insert(std::pair<float, RenderAndUniforms>(priority, paired));
    }

}
