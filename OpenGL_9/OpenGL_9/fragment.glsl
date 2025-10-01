#version 330 core
out vec4 FragColor;

uniform vec3 u_color; // C++에서 보내는 색

void main()
{
    FragColor = vec4(u_color, 1.0); // uniform 색 + 알파값 1.0
}