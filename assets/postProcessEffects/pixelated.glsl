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
    vec2 pixelater = vec2(300, 200);
    vec2 uv = round(fs_in.uv * pixelater) / pixelater;
    FragColor = texture(screen_texture, uv);
}
