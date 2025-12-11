#pragma once

#include "PerlinNoise.h"

#include <vector>
#include <glm/vec3.hpp>
#include <glad/glad.h>
#include "core/FPSCamera.h"

// The Terrain grid represents the terrain as a 3d grid of booleans (basically voxels)
// indicating if they are inside or outside of the terrain
class TerrainGrid {
public:
	TerrainGrid() = delete; // No default constructor, we require dimensions to be provided
	TerrainGrid(glm::ivec3 dimensions, float scale);

	bool get(glm::ivec3) const; // Gets the boolean value at X, Y, Z in the grid
	void set(glm::ivec3, bool newValue); // Sets the boolean value at X, Y, Z in the grid

	void resize(glm::ivec3 newDimensions); // Resizes the grid to new dimensions, while keeping as much of the current contents as possible
	void regenerate(PerlinNoise newNoise); // Regenerate the grid with new Perlin noise terrain
	void clear(); // Clears the grid to air, except for the bottom layer which is solid ground
	void sculpt(glm::ivec3 center, float size, bool destructive);

	void drawDebugPoints(FPSCameraf* camera, GLuint shader, float pointSize); // Draws the grid as points for debugging purposes
	void setDebugPointsRange(glm::ivec3 minIndexes, glm::ivec3 maxIndexes); // Sets a range of indices to draw when using DebugPoints

	int get_x_size() const;
	int get_y_size() const;
	int get_z_size() const;
	glm::ivec3 get_dimensions() const; // Gets all dimensions as a vec
	int get_total_size() const;

	float get_scale() const;
	void set_scale(float newScale);

	PerlinNoise getNoise() const;

private:
	int getIndex(glm::ivec3 p) const;
	void regenerateVBO();

	glm::ivec3 debugPointsRangeMin, debugPointsRangeMax;

	GLuint debug_points_vbo, debug_points_vao;
	int debug_point_count;

	glm::ivec3 dim; // The dimensions of the terrain grid
	float scale;
	std::vector<uint8_t> grid;
	PerlinNoise noise;
};
