//
//  GLSStandardShaders.cpp
//  GLScene
//
//  Created by Arthur MASSON on 4/27/18.
//  Copyright © 2018 Arthur MASSON. All rights reserved.
//

#include "GLSShader.hpp"
#include <iostream>

namespace GLS {
    
    std::shared_ptr<Shader> Shader::standardVertex() {
        std::string src =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;"
        "layout (location = 1) in vec3 aNormal;"
        "layout (location = 2) in vec3 aColor;"
        "layout (location = 3) in vec2 aTexCoord;"
        
        "out vec3 FragPos;"
        "out vec3 Normal;"
        "out vec3 ourColor;"
        "out vec2 texCoord;"
        
        "uniform mat4 projection;"
        "uniform mat4 view;"
        "uniform mat4 model;"
        
        "void main()"
        "{"
        "    gl_Position = projection * view * model * vec4(aPos, 1.0);"
        "    FragPos = vec3(view * model * vec4(aPos, 1.0));"
        "    Normal = mat3(transpose(inverse(view * model))) * aNormal;"
        "    ourColor = aColor;"
        "    texCoord = aTexCoord;"
        "}";
        return std::make_shared<Shader>(src, GL_VERTEX_SHADER);
    }
    
    std::shared_ptr<Shader> Shader::standardFragment() {
        std::string src =
        "#version 330 core\n"
        "out vec4 FragColor;"
        
        "in vec3 FragPos;"
        "in vec3 Normal;"
        "in vec3 ourColor;"
        "in vec2 texCoord;"
        
        "uniform sampler2D texture_diffuse;"
        "uniform sampler2D texture_mask;"
        "uniform int texturebitmask;"
        "uniform float texture_intensity;"
        
        "uniform vec3 view_pos;"
        "uniform vec3 light_ambiant;"
        "uniform float shininess;"
        "uniform vec3 omnilight_position;"
        "uniform vec3 omnilight_color;"
        "uniform int omnilight_isactivated;"
        
        "void main()"
        "{"
        "    if ((texturebitmask & 4) != 0)"
        "    {"
        "        if (texture(texture_mask, texCoord).w == 0)"
        "            discard;"
        "    }"
        "    if ((texturebitmask & 3) == 3)"
        "        FragColor = mix(texture(texture_diffuse, texCoord), vec4(ourColor, 1.0), 0.5);"
        "    else if ((texturebitmask & 2) != 0)"
        "        FragColor = mix(vec4(ourColor, 1.0), texture(texture_diffuse, texCoord), texture_intensity);"
        "    else if ((texturebitmask & 1) != 0)"
        "        FragColor = vec4(ourColor, 1.0);"
        "    else"
        "        FragColor = vec4(1.0);"
        
        "    if (omnilight_isactivated != 0)"
        "    {"
        "        vec3 norm = normalize(Normal);"
        "        vec3 lightDir = normalize(omnilight_position - FragPos);"
        "        float diff = max(dot(norm, lightDir), 0.2);"
        "        vec3 diffuse = diff * omnilight_color;"
        
        "        vec3 viewDir = normalize(view_pos - FragPos);"
        "        vec3 reflectDir = reflect(lightDir, norm);"
        "        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);"
        "        vec3 specular = shininess * spec * omnilight_color;"
        
        "        FragColor = vec4((light_ambiant + diffuse + specular), 1.0) * FragColor;"
        "    }"
        "}";
        return std::make_shared<Shader>(src, GL_FRAGMENT_SHADER);
    }
}
