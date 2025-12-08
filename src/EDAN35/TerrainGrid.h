#pragma once

#include "PerlinNoise.h"

#include <vector>
#include <glm/vec3.hpp>
#include <glad/glad.h>  

// The Terrain grid represents the terrain as a 3d grid of booleans (basically voxels)
// indicating if they are inside or outside of the terrain
class TerrainGrid {
public:
	TerrainGrid() = delete; // No default constructor, we require dimensions to be provided
	TerrainGrid(glm::ivec3 dimensions, float scale);

	bool get(int x, int y, int z) const; // Gets the boolean value at X, Y, Z in the grid
	void set(int x, int y, int z, bool newValue); // Sets the boolean value at X, Y, Z in the grid
	void resize(glm::ivec3 newDimensions); // Resizes the grid to new dimensions, while keeping as much of the current contents as possible
	void regenerate(PerlinNoise newNoise); // Regenerate the grid with new Perlin noise terrain
	void clear(); // Clears the grid to air, except for the bottom layer which is solid ground

	std::pair<GLuint, GLuint> debugPointsVBO(); // Returns the VAO and VBO (in that order) for rendering the grid as points for debugging

	int get_x_size() const;
	int get_y_size() const;
	int get_z_size() const;
	glm::ivec3 get_dimensions() const; // Gets all dimensions as a vec
	int get_total_size() const;

	float get_scale() const;
	void set_scale(float newScale);

	PerlinNoise getNoise() const;

private:
	glm::ivec3 dim; // The dimensions of the terrain grid
	float scale;
	std::vector<uint8_t> grid;
	PerlinNoise noise;
};
