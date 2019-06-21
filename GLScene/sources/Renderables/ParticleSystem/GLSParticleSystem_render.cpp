//
//  GLSParticleSystem_render.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSParticleSystem.hpp"

namespace GLS {

    void ParticleSystem::renderInContext(Scene& scene, const RenderUniforms& uniforms) {
        (void)scene;
        if (!bufferGenerated()) {
            generateBuffers();
            if (!bufferGenerated())
                return;
        }

        std::shared_ptr<ShaderProgram> program;
        program = ShaderProgram::standardShaderProgramDotParticleSystem();
        program->use();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glDisable(GL_STENCIL_TEST);

        uniforms.sendUniformsToShaderProgram(program);

        glUniform3f(program->getLocation("particle_color"), 1, 0, 0);

        glBindVertexArray(_glVertexArray);
        glDrawArrays(GL_POINTS, 0, _properties.count);
    }

    void ParticleSystem::renderInDepthContext(Scene& scene, const RenderUniforms& uniforms) {
        return renderInContext(scene, uniforms);
    }

}
