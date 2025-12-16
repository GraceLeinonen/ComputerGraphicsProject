#version 330 core

// This is a simple shader that takes in a position and a colour
// and converts it to screen-space with that colours

in vec3 colour;
out vec4 frag_color;

void main() {
    frag_color = vec4(colour.xyz, 1.0);
}
