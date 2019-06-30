#version 400 core

uniform mat4 u_mat_projection;
uniform mat4 u_mat_view;
uniform mat4 u_mat_model;
uniform vec3 u_camera_position;
uniform vec2 u_mouse_position;
uniform float u_time;

in VS_OUT {
    vec2 uv;
} fs_in;

uniform sampler2D screen_texture;

out vec4 FragColor;

float distanceToTorus(vec3 p) {
    p.x += sin(u_time);
    vec2 t = vec2(1, 0.3);
    vec2 q = vec2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

float distanceToSphere(vec3 position) {
    vec3 sphereCenter = vec3(0, cos(u_time / 5), 0);
    float sphereRadius = 0.5;
    return length(position - sphereCenter) - sphereRadius;
}

float distanceToCube(vec3 position) {
    vec3 b = vec3(1, 1, 1);
    vec3 d = abs(position) - b;
    return length(max(d, 0)) + min(max(d.x, max(d.y, d.z)), 0);
}

float distanceToFractal(vec3 p) {
    p *= 0.5;
    float power = 5;
    vec3 z = p;
    float dr = 1;
    float r;

    for (int i = 0; i < 15; i++) {
        r = length(z);
        if (r > 2)
            break;
        
        float theta = acos(z.z / r) * power;
        float phi = atan(z.y, z.x) * power;
        float zr = pow(r, power);
        dr = pow(r, power - 1) * power * dr + 1;
        z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
        z += p;
    }
    return 0.5 * log(r) * r / dr;
}

float smoothMin(float a, float b, float k) {
    float h = max(k - abs(a - b), 0) / k;
    return min(a, b) - h * h * h * k * 1 / 6.0;
}

float distanceToScene(vec3 position) {
    // float d = distanceToTorus(position.yxz);
    // d = smoothMin(d, distanceToCube(position), 0.2);
    // d = smoothMin(d, distanceToSphere(position), 0.2);
    // return d;
    return distanceToFractal(position);
}

void main() {

    vec2 devPos = (fs_in.uv * vec2(2)) - vec2(1); 
    vec3 rayOrigin = u_camera_position;
    vec3 rayDirection = normalize(vec3(inverse(u_mat_view) * vec4(normalize(vec3(devPos, -1)), 0)));

    float closest = 5;
    float occlusion;
    for (int iterations = 0; iterations < 100; iterations++) {
        float d = distanceToScene(rayOrigin);
        if (d < closest)
            closest = d;
        if (d > 30) {
            FragColor = texture(screen_texture, fs_in.uv) + (1 - closest) / 1 * vec4(1);
            return;
        }
        rayOrigin += rayDirection * d;
        if (d <= 0.01) {
            occlusion = iterations / 100.0;
            break;
        }
    }
    FragColor = (1 - occlusion) * vec4(1);
}
