#version 330 core

layout(location = 0) in vec3 world_position;
layout(location = 1) in float colourFlagIN;

out float colourFlag;
uniform mat4 projection;

void main()
{
	colourFlag = colourFlagIN; // Send the colour flag to the fragment shader.
    gl_Position = projection * vec4(world_position, 1.0);
}
