#version 330 core

out vec4 color;

uniform vec3 FragPos;

void main() {
	color = vec4(FragPos, 1.0);
}