#version 330 core

uniform mat4 projection;

layout(location = 0) in vec3 world_position;
layout(location = 1) in vec3 normal;

out VS_OUT {
    vec3 world_position;
    float world_y;
    vec3 normal;
} vs_out;

void main() {

    vs_out.world_position = world_position;
    vs_out.world_y = world_position.y;
    vs_out.normal = normal;

    gl_Position = projection * vec4(world_position, 1.0);
};