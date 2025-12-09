#include "TerrainGrid.h"
#include "core/Bonobo.h"

TerrainGrid::TerrainGrid(glm::ivec3 dimensions, float scale)
	: dim(dimensions), grid(dimensions.x * dimensions.y * dimensions.z, false), scale(scale), noise(PerlinNoise(0, 0.1f)) // Set the x, y and z sizes, and initiallise the grid to all empty values
{
	regenerate(noise); // Generate the terrain immediately with the current noise function
}

bool TerrainGrid::get(int x, int y, int z) const {
	return grid[x + y * dim.x + z * dim.x * dim.y];
}

void TerrainGrid::set(int x, int y, int z, bool newValue) {
	grid[x + y * dim.x + z * dim.x * dim.y] = newValue;
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
	scale = newScale;
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
}

std::pair<GLuint, GLuint> TerrainGrid::debugPointsVBO() {
	return debugPointsVBOWithDimensions(glm::ivec3(0), dim);
}


std::pair<GLuint, GLuint> TerrainGrid::debugPointsVBOWithDimensions(glm::ivec3 minIndexes, glm::ivec3 maxIndexes) {
	LogInfo("Creating a new VAO/VBO for the debug points of the terrain grid.");

	// For each of the points, add them to a float array and add a colour flag
	std::vector<float> points;
	for (int x = minIndexes.x; x < glm::min(dim.x, maxIndexes.x); x++)
		for (int y = minIndexes.y; y < glm::min(dim.y, maxIndexes.y); y++)
			for (int z = minIndexes.z; z < glm::min(dim.z, maxIndexes.z); z++) {
				points.push_back((float)x * scale);
				points.push_back((float)y * scale);
				points.push_back((float)z * scale);
				if (get(x, y, z)) {
					points.push_back(1.0f); // 0x1 colour flag to indicate a terrain voxel
				}
				else {
					points.push_back(0.0f); // 0x0 colour flag to indicate an empty voxel
				}
			}


	// Generate the VAO and VBO
	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	// Bind the right buffers
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Add the data to the buffers
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // the world position data (at pos=0) needs 3 floats of data
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float))); // the colour flag data (at pos=1) needs 1 float of data
	glEnableVertexAttribArray(0); // Enable position at pos=0
	glEnableVertexAttribArray(1); // Enable colour flag at pos=1

	// Unbind the buffers (so that the next code doesn't accidentically override it)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return { vao, vbo };
}


void TerrainGrid::regenerate(PerlinNoise newNoise) {
	LogInfo("Regenerating the terrain  with perlin noise");
	noise = newNoise; // Save the noise function in case we need to generate more later (if the grid is resized)

	for (int x = 0; x < dim.x; x++) {
		for (int z = 0; z < dim.z; z++) {
			float noise_height = noise.sampleNoise(x, z); // Sample the height between 0-1 at this position
			noise_height = floor(noise_height * dim.y); // Scale it by our max Y height and floor this
			// Set the voxels to true as long as they are below or equal to this noise height
			for (int y = 0; y < dim.y; y++) {
				if (y <= noise_height) {
					set(x, y, z, true);
				}
				else {
					set(x, y, z, false);
				}
			}
		}
	}
}

void TerrainGrid::resize(glm::ivec3 newDimensions) {
	LogInfo("Resizing to %i, %i, %i", newDimensions.x, newDimensions.y, newDimensions.z);
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
				grid[x + y * dim.x + z * dim.x * dim.y] = original[x + y * oldDim.x + z * oldDim.x * oldDim.y];
			}
		}
	}
}

void TerrainGrid::generateDensity() {

	density.resize(dim.x);
	for (int x = 0; x < dim.x; ++x) {
		density[x].resize(dim.y);
		for (int y = 0; y < dim.y; ++y) {
			density[x][y].resize(dim.z);
			for (int z = 0; z < dim.z; ++z) {

				float height = getNoise().sampleNoise(x,z) * dim.y; // a value between 0-1 * height of grid
				density[x][y][z] = y - height;

				// when y < height, inside terrain
				// when y == height, on terrain
				// when y > height, above terrain
				// algorithm will generate triangles at locations where density = isoLevel
			}
		}
	}
}
