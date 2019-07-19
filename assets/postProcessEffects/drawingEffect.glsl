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

float calculEdgeValue() {
    const float offset = 1.0 / 300.0;
    const vec2 offsets[9] = vec2[](
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

    const float kernel[9] = float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
        sampleTex[i] = vec3(texture(screen_texture, fs_in.uv + offsets[i]));
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    float seuil = 0.2;
    return col.x < seuil && col.y < seuil && col.z < seuil ? 0 : 1;
}

void main() {
    vec3 color = texture(screen_texture, fs_in.uv).xyz;
    float hashing = 10;
    color = floor(color * hashing) / hashing;
    FragColor = vec4(color - vec3(calculEdgeValue()), 1.0);
}
