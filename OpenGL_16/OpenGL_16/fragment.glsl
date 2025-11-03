#version 330 core
in vec3 vColor;
out vec4 FragColor;

uniform vec3 u_color; // CPU에서 전달하는 면 색

void main()
{
    FragColor = vec4(vColor, 1.0);
}