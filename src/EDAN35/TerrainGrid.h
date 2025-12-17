#pragma once

#include "PerlinNoise.h"
#include <vector>
#include <glm/vec3.hpp>
#include <glad/glad.h>
#include "core/FPSCamera.h"
#include <functional>

// The Terrain grid represents the terrain as a 3d grid of booleans (basically voxels)
// indicating if they are inside or outside of the terrain
class TerrainGrid {
public:
	TerrainGrid() = delete; // No default constructor, we require dimensions to be provided
	TerrainGrid(glm::ivec3 dimensions, float scale);

	float get(glm::ivec3) const; // Gets the boolean value at X, Y, Z in the grid
	void set(glm::ivec3, float newValue); // Sets the boolean value at X, Y, Z in the grid

	void resize(glm::ivec3 newDimensions); // Resizes the grid to new dimensions, while keeping as much of the current contents as possible
	void regenerate(PerlinNoise newNoise); // Regenerate the grid with new Perlin noise terrain
	void clear(); // Clears the grid to air, except for the bottom layer which is solid ground
	void sculpt(glm::ivec3 center, FPSCameraf* camera, float size, float strength, bool destructive);

	void registerUpdateCallback(std::function<void()> callback); // Registers a callback to be called whenever the grid is updated

	glm::ivec3 getDimensions() const; // Gets all dimensions as a vec
	int getTotalSize() const; // total amount of voxels = max_x * max_y * max_z

	// The Scale factor is the VISUAL distance between voxels. The underlying calculations (such as sculpting raycasting) still happen at scale == 1.
	float getScale() const;
	void setScale(float newScale); 

	PerlinNoise getNoise() const;

private:
	void updatedTerrain();
	int getIndex(glm::ivec3 p) const;

	std::vector<std::function<void()>> updateCallbacks;

	glm::ivec3 dim; // The dimensions of the terrain grid
	float scale;
	std::vector<float> grid; // The actual underlying terrain data

	PerlinNoise noise; // The PerlinNoise that should be used to generate more terrain
};
