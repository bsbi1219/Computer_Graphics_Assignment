#version 330 core
out vec4 FragColor;
in vec3 vColor;

uniform vec3 u_color; // CPU에서 전달하는 면 색

void main()
{
    FragColor = vec4(vColor, 1.0);
}