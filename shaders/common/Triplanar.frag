#version 330 core

uniform vec3 light_direction;
uniform vec3 camera_position;
uniform float max_y;

uniform sampler2D textureMap[2];

in VS_OUT {
    vec3 world_position;
	float world_y;
	vec3 normal;
    vec2 texcoord;
} fs_in;

out vec4 FragColor;

void main() {

    // define colors
    // vec4 snow = vec4(1.0, 1.0, 1.0, 1.0);
    // vec4 grass = vec4(0.4, 0.49, 0.36, 1.0);
    vec4 snow = texture(textureMap[0], fs_in.texcoord);
    vec4 grass = texture(textureMap[1], fs_in.texcoord);

    // define snow start and end
    float snow_start = max_y * 0.6;
    float snow_end = max_y * 0.8;

    // blending
    float alpha = smoothstep(snow_start, snow_end, fs_in.world_y);

    // Add diffuse and specular lighting component
    vec4 diffuse_color = mix(grass, snow, alpha);
    vec4 specular_color = vec4(1.0, 1.0, 1.0, 1.0);

    vec3 L = normalize(light_direction);
    vec3 N = normalize(fs_in.normal);
    vec3 V = normalize(camera_position - fs_in.world_position);
    vec3 R = reflect(-L, N);

    float diffuse = max(dot(N,-L), 0.0);
    float specular = pow(max(dot(R,V), 0.0), 10.0);

    FragColor = diffuse_color * diffuse + specular_color * specular;

};
