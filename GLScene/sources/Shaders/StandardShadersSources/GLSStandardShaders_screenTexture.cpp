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

        // Normal
        // "void main() {\n"
        // "   FragColor = texture(screen_texture, fs_in.uv);\n"
        // "}\n"

        // Grayscale
        "void main() {\n"
        "   FragColor = texture(screen_texture, fs_in.uv);\n"
        "   float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;\n"
        "   FragColor = vec4(vec3(average), 1.0);\n"
        "}\n"

        // Kernel effect
        // "void main() {"
        // "   const float offset = 1.0 / 300.0;"
        // "   vec2 offsets[9] = vec2[]("
        // "       vec2(-offset,  offset)," // top-left
        // "       vec2( 0.0f,    offset)," // top-center
        // "       vec2( offset,  offset)," // top-right
        // "       vec2(-offset,  0.0f),  " // center-left
        // "       vec2( 0.0f,    0.0f),  " // center-center
        // "       vec2( offset,  0.0f),  " // center-right
        // "       vec2(-offset, -offset)," // bottom-left
        // "       vec2( 0.0f,   -offset)," // bottom-center
        // "       vec2( offset, -offset) " // bottom-right    
        // "   );"

        // // "   float kernel[9] = float[](" // Simple
        // // "       -1, -1, -1,"
        // // "       -1,  9, -1,"
        // // "       -1, -1, -1"
        // // "   );"

        // "   float kernel[9] = float[](" // Edge
        // "       1, 1, 1,"
        // "       1, -8, 1,"
        // "       1, 1, 1"
        // "   );"

        // // "   float kernel[9] = float[](" // Blur
        // // "       1.0 / 16, 2.0 / 16, 1.0 / 16,"
        // // "       2.0 / 16, 4.0 / 16, 2.0 / 16,"
        // // "       1.0 / 16, 2.0 / 16, 1.0 / 16"
        // // "   );"

        // "   vec3 sampleTex[9];"
        // "   for(int i = 0; i < 9; i++)"
        // "       sampleTex[i] = vec3(texture(screen_texture, fs_in.uv.st + offsets[i]));"
        // "   vec3 col = vec3(0.0);"
        // "   for(int i = 0; i < 9; i++)"
        // "   col += sampleTex[i] * kernel[i];"
        // "   FragColor = vec4(col, 1.0);"
        // "}" 

        "\n";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }

}
