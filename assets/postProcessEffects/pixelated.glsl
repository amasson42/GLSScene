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
    vec2 pixelater = vec2(600, 400);
    vec2 uv = round(fs_in.uv * pixelater) / pixelater;
	// vec4 color = vec4(0);
	// vec2 offset = vec2(0);
	// int total = 0;
	// for (offset.x = 0; offset.x < 1.0 / pixelater.x; offset.x += 0.1 / pixelater.x) {
	// 	for (offset.y = 0; offset.y < 1.0 / pixelater.y; offset.y += 0.1 / pixelater.y) {
	// 		color += texture(screen_texture, uv + offset);
	// 		total++;
	// 	}
	// }
	// FragColor = color / total;
    FragColor = texture(screen_texture, uv);
}
