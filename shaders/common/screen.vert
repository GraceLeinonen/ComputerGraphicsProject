#version 330 core

layout(location = 0) in vec2 screen_position;

uniform mat4 projection;

out float world_y; // Send the world_y position to the fragment shader

void main()
{
    gl_Position = vec4(screen_position, 0.0f, 1.0f);
}
