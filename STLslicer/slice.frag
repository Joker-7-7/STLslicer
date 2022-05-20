#version 330 core

out vec4 FragColor;
uniform vec4 OurColor;

void main()
{
    FragColor = OurColor;
}