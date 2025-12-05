#version 330 core

in float world_y;
out vec4 frag_color;

void main() {

	float max_y = 5;
	float min_y = 0;

	vec3 rgb = vec3(0.0f);

	// If the pixel is below the allowed Y range, show that with a magenta colour
	if (world_y > max_y || world_y < min_y) {
		rgb = vec3(1.0f, 0.0f, 1.0f);
	} else {
		float blue = clamp((world_y - min_y) / (max_y - min_y), 0, 1);
		rgb = vec3(1.0f - blue, 1.0f - blue, 1.0f);
	}

    frag_color = vec4(rgb.xyz, 1.0);
}
