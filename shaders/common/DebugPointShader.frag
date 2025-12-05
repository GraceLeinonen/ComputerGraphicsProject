#version 330 core

in float colourFlag;
out vec4 FragColor;

void main() {
    if (colourFlag == 0.0f)
		FragColor = vec4(0.0, 0.0, 0.0, 1.0); // black
	else
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // white
}
