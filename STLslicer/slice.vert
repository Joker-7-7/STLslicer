#version 330 core
layout (location = 0) in vec3 vert;

uniform mat4 model;
uniform mat4 proj;
uniform mat4 view;

void main() {
    gl_Position = proj * model* view * vec4(vert, 1.0f);
}