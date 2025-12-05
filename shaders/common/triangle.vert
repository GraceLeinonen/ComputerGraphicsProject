#version 330 core

layout(location = 0) in vec3 world_position;

uniform mat4 projection;

out float world_y; // Send the world_y position to the fragment shader

void main()
{
	world_y = world_position.y;
    gl_Position = projection * vec4(world_position, 1.0);
}
