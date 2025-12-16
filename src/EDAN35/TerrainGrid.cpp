#include "TerrainGrid.h"
#include "core/Bonobo.h"
#include <glm/gtc/type_ptr.hpp>

TerrainGrid::TerrainGrid(glm::ivec3 dimensions, float scale)
	: dim(dimensions), grid(dimensions.x * dimensions.y * dimensions.z, false),
	scale(scale), noise(PerlinNoise(0, 0.05f)), debug_point_count(dimensions.x * dimensions.y * dimensions.z), // Set the x, y and z sizes, and initiallise the grid to all empty values
	debug_points_vao(0), debug_points_vbo(0), debugPointsRangeMax(scale), debugPointsRangeMin(glm::vec3(0))
{
	regenerate(noise); // Generate the terrain immediately with the current noise function
	updatedTerrain();
}

bool TerrainGrid::get(glm::ivec3 p) const {
	if (p.x < 0 || p.x >= get_x_size()
		|| p.y < 0 || p.y >= get_y_size()
		|| p.z < 0 || p.z >= get_z_size()) {
		// If the position is out of bounds, return false
		return false;
	}
	return grid[getIndex(p)];
}

void TerrainGrid::set(glm::ivec3 p, bool newValue) {
	if (p.x < 0 || p.x >= get_x_size()
		|| p.y < 0 || p.y >= get_y_size()
		|| p.z < 0 || p.z >= get_z_size()) {
		// If the position is out of bounds, dont set it
		return;
	}

	grid[getIndex(p)] = newValue;
}

int TerrainGrid::getIndex(glm::ivec3 p) const {
	return p.x + p.y * dim.x + p.z * dim.x * dim.y;
}

int TerrainGrid::get_x_size() const {
	return dim.x;
}

int TerrainGrid::get_y_size() const {
	return dim.y;
}

int TerrainGrid::get_z_size() const {
	return dim.z;
}

glm::ivec3 TerrainGrid::get_dimensions() const {
	return dim;
}

float TerrainGrid::get_scale() const {
	return scale;
}

int TerrainGrid::get_total_size() const {
	return get_x_size() * get_y_size() * get_z_size();
}

void TerrainGrid::set_scale(float newScale) {
	if (scale == newScale) return;
	scale = newScale;

	// Since the scale changed, we now have to regenerate the VBO
	updatedTerrain();
}

PerlinNoise TerrainGrid::getNoise() const {
	return noise;
}

void TerrainGrid::clear() {
	for (int i = 0; i < grid.size(); i++) {
		grid[i] = false;
	}
	for (int x = 0; x < dim.x; x++) {
		for (int z = 0; z < dim.z; z++) {
			grid[x + z * dim.x * dim.y] = true;
		}
	}
	updatedTerrain();
}

void TerrainGrid::sculpt(glm::ivec3 center, float size, bool destructive) {

	// Sculpt the terrain in a sphere around the hit:

	// Loop through all coordinates that might be within the circle (size / 2 cube around the center)

	float radiusSquared = size * size;
	for (int x = center.x - size; x <= center.x + size; x++) {
		for (int y = center.y - size; y <= center.y + size; y++) {
			for (int z = center.z - size; z <= center.z + size; z++) {
				if (glm::pow(x - center.x, 2) + glm::pow(y - center.y, 2) + glm::pow(z - center.z, 2) <= radiusSquared) {
					set(glm::ivec3(x, y, z), !destructive);
				}
			}
		}
	}

	// Regenerate the VBO, since the grid changed
	updatedTerrain();
}

void TerrainGrid::registerUpdateCallback(std::function<void()> callback) {
	updateCallbacks.push_back(callback);
}

void TerrainGrid::updatedTerrain() {
	// Call back the callbacks
	for (auto& callback : updateCallbacks) {
		callback();
	}
}


void TerrainGrid::regenerate(PerlinNoise newNoise) {
	LogInfo("Regenerating the terrain with perlin noise");
	noise = newNoise; // Save the noise function in case we need to generate more later (if the grid is resized)

	for (int x = 0; x < dim.x; x++) {
		for (int z = 0; z < dim.z; z++) {
			float noise_height = noise.sampleNoise(x, z); // Sample the height between 0-1 at this position
			noise_height = floor(noise_height * dim.y); // Scale it by our max Y height and floor this
			// Set the voxels to true as long as they are below or equal to this noise height
			for (int y = 0; y < dim.y; y++) {
				if (y <= noise_height) {
					set(glm::vec3(x, y, z), true);
				}
				else {
					set(glm::vec3(x, y, z), false);
				}
			}
		}
	}

	// Regenerate the VBO since the grid has changed
	updatedTerrain();
}

void TerrainGrid::resize(glm::ivec3 newDimensions) {
	std::vector<uint8_t> original = grid; // Copy the current grid

	glm::ivec3 oldDim = dim; // Save the old dimensions
	dim = newDimensions; // Set the dimensions
	grid = std::vector<uint8_t>(dim.x * dim.y * dim.z, false); // Actually create a grid the right size
	regenerate(noise); // Use the regenerate to generate an entire grid based on the current noise pattern

	// If Y changed, we can't keep the edited map since we would have to stretch/squash it along the y-axis
	if (oldDim.y != dim.y) return;
	// Add as much of the original grid back as possible
	for (int x = 0; x < glm::min(dim.x, oldDim.x); x++) {
		for (int y = 0; y < glm::min(dim.y, oldDim.y); y++) {
			for (int z = 0; z < glm::min(dim.z, oldDim.z); z++) {
				grid[getIndex(glm::vec3(x, y, z))] = original[x + y * oldDim.x + z * oldDim.x * oldDim.y];
			}
		}
	}

	// Regenerate the VBO since the grid has changed
	updatedTerrain();
}
