//
//  GLSStandardShaders_skybox.cpp
//  GLScene
//
//  Created by Arthur MASSON on 4/27/18.
//  Copyright © 2018 Arthur MASSON. All rights reserved.
//

#include "GLSShader.hpp"
#include "GLSSkybox.hpp"

namespace GLS {

    std::shared_ptr<Shader> Shader::standardVertexSkybox() {
        std::string src =
        "#version 400 core\n\n"

        + Skybox::shaderUniformsVertex() +

        "void main() {\n"
        "    vs_out.uv = position;\n"
        "    vec4 pos = u_mat_projection * u_mat_view * vec4(position, 1.0);\n"
        "    gl_Position = pos.xyww;\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_VERTEX_SHADER);
    }

    std::shared_ptr<Shader> Shader::standardFragmentSkybox() {
        std::string src =
        "#version 400 core\n\n"

        + Skybox::shaderUniformsFragment() +

        "void main() {\n"
        "    FragColor = texture(u_skybox, normalize(fs_in.uv));\n"
        "}\n"
        "\n";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }

}
