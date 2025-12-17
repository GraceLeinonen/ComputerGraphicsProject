#version 330 core

in float colourFlag;
out vec4 FragColor;

void main() {
	FragColor = vec4(colourFlag, colourFlag, colourFlag, 1.0);
}
