#version 400 core

uniform mat4 u_mat_projection;
uniform mat4 u_mat_view;
uniform mat4 u_mat_model;
uniform vec3 u_camera_position;
uniform vec2 u_mouse_position;

in VS_OUT {
    vec2 uv;
} fs_in;

uniform sampler2D screen_texture;

out vec4 FragColor;


float random (float f) {
    return fract(2985645.0 * sin(f));
}

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

float mix4(float v[4], vec2 p) {
    vec4 c = vec4(1.0 - distance(vec2(0.0, 0.0), p),
                  1.0 - distance(vec2(1.0, 0.0), p),
                  1.0 - distance(vec2(0.0, 1.0), p),
                  1.0 - distance(vec2(1.0, 1.0), p));
    return (v[0] * c[0] +
            v[1] * c[1] +
            v[2] * c[2] +
            v[3] * c[3]) / (c[0] + c[1] + c[2] + c[3]);
}

vec4 mix4(vec4 v[4], vec2 p) {
    // 2 3
    // 0 1
    vec4 c = vec4(1.0 - distance(vec2(0.0, 0.0), p),
                  1.0 - distance(vec2(1.0, 0.0), p),
                  1.0 - distance(vec2(0.0, 1.0), p),
                  1.0 - distance(vec2(1.0, 1.0), p));
    return (v[0] * c[0] +
            v[1] * c[1] +
            v[2] * c[2] +
            v[3] * c[3]) / (c[0] + c[1] + c[2] + c[3]);
}

float noise(float f) {
      float a = random(floor(f));
      float b = random(floor(f) + 1.0);
      return mix(a, b, smoothstep(0., 1., fract(f)));
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    
    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    
    // Smooth Interpolation
    
    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);
    
    float v[4];
    v[0] = a;
    v[1] = b;
    v[2] = c;
    v[3] = d;
    // Mix 4 coorners percentages
//    return mix4(v, u);
    return mix(a, b, u.x) +
    (c - a)* u.y * (1.0 - u.x) +
    (d - b) * u.x * u.y;
}

void main() {

    vec2 wpos = fs_in.uv + u_mouse_position * 100;
    wpos *= vec2(10);

    float nValue1 = noise(wpos * vec2(1, 1) + vec2(235.4, 12.9));
    float nValue2 = 0.1 * noise(wpos * vec2(3.4, 1.8) + vec2(134, 653));
    float nValue3 = 0.5 * noise(wpos * vec2(0.1, 0.1));

    float nValueTotal = (nValue1 + nValue2 + nValue3);

    FragColor = vec4(vec3(nValueTotal), 1);
    // FragColor = texture(screen_texture, fs_in.uv);
}
