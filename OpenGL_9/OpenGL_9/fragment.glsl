#version 330 core
out vec4 FragColor;

uniform vec3 u_color; // C++���� ������ ��

void main()
{
    FragColor = vec4(u_color, 1.0); // uniform �� + ���İ� 1.0
}