//
//  GLSStandardShaders_screenTexture.cpp
//  GLScene
//
//  Created by Arthur MASSON on 4/27/18.
//  Copyright © 2018 Arthur MASSON. All rights reserved.
//

#include "GLSShader.hpp"

namespace GLS {

    std::shared_ptr<Shader> Shader::standardVertexScreenTexture() {
        std::string src =
        "#version 400 core\n\n"

        + Framebuffer::shaderUniformsVertex() +

        "void main() {\n"
        "    gl_Position = vec4(position.x, position.y, 0, 1);\n"
        "    vs_out.uv = (position + vec2(1.0)) / 2.0;\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_VERTEX_SHADER);
    }

    std::shared_ptr<Shader> Shader::standardFragmentScreenTexture() {
        std::string src =
        "#version 400 core\n\n"

        + Framebuffer::shaderUniformsFragment() +

        "void main() {\n"
        "   FragColor = texture(screen_texture, fs_in.uv);\n"
        "}\n"

        "\n";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }

}
