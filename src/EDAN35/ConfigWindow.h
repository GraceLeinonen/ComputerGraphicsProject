#pragma once

#include <glm/vec3.hpp>
#include "TerrainGrid.h"


// The config is an object representation of the state of the Scene Controls window
// It is in its own object to keep the main() function a bit more clean
class Config {
public:
	Config() = delete; // No default constructor, we require a TerrainGrid to be provided
	Config(TerrainGrid* terrain);
	void draw_config();

	glm::ivec3 terrain_dimensions; // The amount of voxels in the terrain grid
	float terrain_scale; // The distance between each voxel in the terrain grid
	bool terrain_updated; // Is TRUE on the frame where the terrain is updated (and so VAO/VBOs need to be regenerated)

	bool pd_show_points_debugger; // pd_ = points_debugger_
	float pd_point_size;

	bool bd_show_basis; // bd_ = basis_debugger_
	float bd_thickness;
	float bd_length;

	int pn_seed; // pn_ = perlin_noise_
	float pn_scale;
private:
	TerrainGrid* terrain;
};
