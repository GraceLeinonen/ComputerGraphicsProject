#include "TerrainGrid.h"
#include "core/Bonobo.h"
#include <glm/gtc/type_ptr.hpp>

TerrainGrid::TerrainGrid(glm::ivec3 dimensions, float scale)
	: dim(dimensions), grid(dimensions.x * dimensions.y * dimensions.z, false),
	scale(scale), noise(PerlinNoise(0, 0.05f))
{
	regenerate(noise); // Generate the terrain immediately with the current noise function
	updatedTerrain();
}

float TerrainGrid::get(glm::ivec3 p) const {
	if (p.x < 0 || p.x >= dim.x
		|| p.y < 0 || p.y >= dim.y
		|| p.z < 0 || p.z >= dim.z) {
		// If the position is out of bounds, return false
		return 0;
	}
	return grid[getIndex(p)];
}

void TerrainGrid::set(glm::ivec3 p, float newValue) {
	if (p.x < 0 || p.x >= dim.x
		|| p.y < 0 || p.y >= dim.y
		|| p.z < 0 || p.z >= dim.z) {
		// If the position is out of bounds, dont set it
		return;
	}
	if (newValue < 0) {
		newValue = 0;
	}
	if (newValue > 1) {
		newValue = 1;
	}

	grid[getIndex(p)] = newValue;
}

int TerrainGrid::getIndex(glm::ivec3 p) const {
	return p.x + p.y * dim.x + p.z * dim.x * dim.y;
}

glm::ivec3 TerrainGrid::getDimensions() const {
	return dim;
}

float TerrainGrid::getScale() const {
	return scale;
}

int TerrainGrid::getTotalSize() const {
	return dim.x * dim.y * dim.z;
}

void TerrainGrid::setScale(float newScale) {
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

void TerrainGrid::sculpt(glm::ivec3 center, FPSCameraf* camera, float size, float strength, bool destructive) {
	// Sculpt the terrain in a sphere around the hit:
	float radiusSquared = size * size;
	float centerDepth = glm::length(glm::vec3(center) - camera->mWorld.GetTranslation());
	float offset = 0.4f; // Small offset to allow sculpting a bit behind the terrain

	for (int x = center.x - size; x <= center.x + size; x++) {
		for (int y = center.y - size; y <= center.y + size; y++) {
			for (int z = center.z - size; z <= center.z + size; z++) {
				if (glm::pow(x - center.x, 2) + glm::pow(y - center.y, 2) + glm::pow(z - center.z, 2) <= radiusSquared) {
					float depth = glm::length(glm::vec3(x, y, z) - camera->mWorld.GetTranslation());

					if (destructive && depth < centerDepth + offset) { // Only modify voxels that are closer to the camera than the target we hit
						set(glm::ivec3(x, y, z), get(glm::ivec3(x, y, z)) - strength);
					}
					else if (!destructive) { // Only modify voxels that are further to the camera than the target we hit
						set(glm::ivec3(x, y, z), get(glm::ivec3(x, y, z)) + strength);
					}
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
					if (y >= noise_height - 1) {
						set(glm::vec3(x, y, z), noise_height - y);
					}
					else {
						set(glm::vec3(x, y, z), 1);
					}
				}
				else {
					set(glm::vec3(x, y, z), 0);
				}
			}
		}
	}

	// Regenerate the VBO since the grid has changed
	updatedTerrain();
}

void TerrainGrid::resize(glm::ivec3 newDimensions) {
	std::vector<float> original = grid; // Copy the current grid

	glm::ivec3 oldDim = dim; // Save the old dimensions
	dim = newDimensions; // Set the dimensions
	grid = std::vector<float>(dim.x * dim.y * dim.z, false); // Actually create a grid the right size
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
