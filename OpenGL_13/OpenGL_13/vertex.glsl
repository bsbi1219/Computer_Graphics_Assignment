#version 330 core
layout(location = 0) in vec2 position;
uniform vec2 u_center;
uniform float u_scale;

void main() {
    vec2 scaled = (position - u_center) * u_scale + u_center;
    gl_Position = vec4(scaled, 0.0, 1.0);
}
