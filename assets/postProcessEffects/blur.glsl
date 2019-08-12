#version 400 core

uniform mat4 u_mat_projection;
uniform mat4 u_mat_view;
uniform mat4 u_mat_model;
uniform vec3 u_camera_position;

in VS_OUT {
    vec2 uv;
} fs_in;

uniform sampler2D screen_texture;

out vec4 FragColor;

void main() {
    const float offset = 1.0 / 500.0;
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset),
        vec2( 0.0f,    offset),
        vec2( offset,  offset),
        vec2(-offset,  0.0f),
        vec2( 0.0f,    0.0f),
        vec2( offset,  0.0f),
        vec2(-offset, -offset),
        vec2( 0.0f,   -offset),
        vec2( offset, -offset)
    );

    float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
        sampleTex[i] = vec3(texture(screen_texture, fs_in.uv.st + offsets[i]));
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
    col += sampleTex[i] * kernel[i];
    FragColor = vec4(col, 1.0);
}
