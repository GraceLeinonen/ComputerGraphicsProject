#version 330 core

// This is a simple shader that takes in a position and a colour
// and converts it to screen-space with that colour

layout(location = 0) in vec3 world_position;
layout(location = 1) in vec3 colourIN; 

uniform mat4 projection;

out vec3 colour; 

void main()
{
	colour = colourIN; // Send the colour to the vertex shader
    gl_Position = projection * vec4(world_position, 1.0);
}
