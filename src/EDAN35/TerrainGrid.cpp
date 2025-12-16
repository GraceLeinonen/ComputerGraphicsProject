#include "TerrainGrid.h"
#include "core/Bonobo.h"
#include <glm/gtc/type_ptr.hpp>

TerrainGrid::TerrainGrid(glm::ivec3 dimensions, float scale)
	: dim(dimensions), grid(dimensions.x * dimensions.y * dimensions.z, false),
	scale(scale), noise(PerlinNoise(0, 0.1f)), debug_point_count(dimensions.x * dimensions.y * dimensions.z), // Set the x, y and z sizes, and initiallise the grid to all empty values
	debug_points_vao(0), debug_points_vbo(0), debugPointsRangeMax(scale), debugPointsRangeMin(glm::vec3(0))
{
	regenerate(noise); // Generate the terrain immediately with the current noise function
	regenerateVBO();
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
	regenerateVBO();
}

PerlinNoise TerrainGrid::getNoise() const {
	return noise;
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
	regenerateVBO();
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
	regenerateVBO();
}

void TerrainGrid::drawDebugPoints(FPSCameraf* camera, GLuint shader, float pointSize) {
	// If there is no VBO yet, create one
	if (debug_points_vbo == 0) {
		regenerateVBO();
	}

	glUseProgram(shader); // Use the debug point shader
	// Provide the projection matrix to the shader
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(camera->GetWorldToClipMatrix()));

	glBindVertexArray(debug_points_vao);
	glPointSize(pointSize);

	glDrawArrays(GL_POINTS, 0, debug_point_count);
	glBindVertexArray(0);
	glUseProgram(0);
}

void TerrainGrid::setDebugPointsRange(glm::ivec3 minIndexes, glm::ivec3 maxIndexes) {
	// Ensure the min and max are the min and the max
	glm::ivec3 min = glm::min(minIndexes, maxIndexes);
	glm::ivec3 max = glm::max(minIndexes, maxIndexes);

	// Clamp the min and max to legal values
	min = glm::clamp(min, glm::ivec3(0), dim);
	max = glm::clamp(max, glm::ivec3(0), dim);
	int count = (max.x - min.x) * (max.y - min.y) * (max.z - min.z);

	if (min == debugPointsRangeMin && max == debugPointsRangeMax && count == debug_point_count) {
		// No change, so we dont have to regenerate the VBO
		return;
	}

	// Save the ranges
	debugPointsRangeMin = min;
	debugPointsRangeMax = max;

	debug_point_count = count;

	// Regenerate the vbo
	regenerateVBO();
}

void TerrainGrid::regenerateVBO() {
	LogInfo("Regenerating the VBO for the terrain grid debug points");

	// If we still have old debug points, delete them
	if (debug_points_vbo != 0) {
		glDeleteBuffers(1, &debug_points_vbo);
		glDeleteVertexArrays(1, &debug_points_vao);
	}

	// Generate the new VAO and VBO
	glGenBuffers(1, &debug_points_vbo);
	glGenVertexArrays(1, &debug_points_vao);

	// Bind the VAO and VBO
	glBindVertexArray(debug_points_vao);
	glBindBuffer(GL_ARRAY_BUFFER, debug_points_vbo);

	// Load all the points into a float array
	std::vector<float> points;
	for (int x = debugPointsRangeMin.x; x < debugPointsRangeMax.x; x++)
		for (int y = debugPointsRangeMin.y; y < debugPointsRangeMax.y; y++)
			for (int z = debugPointsRangeMin.z; z < debugPointsRangeMax.z; z++) {
				points.push_back((float)x * scale);
				points.push_back((float)y * scale);
				points.push_back((float)z * scale);
				if (get(glm::ivec3(x, y, z))) {
					points.push_back(1.0f); // 0x1 colour flag to indicate a terrain voxel
				}
				else {
					points.push_back(0.0f); // 0x0 colour flag to indicate an empty voxel
				}
			}

	// Move the data to the VBO
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);

	// Load the attribute information into the VAO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // the world position data (at pos=0) needs 3 floats of data
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float))); // the colour flag data (at pos=1) needs 1 float of data
	glEnableVertexAttribArray(0); // Enable position at pos=0
	glEnableVertexAttribArray(1); // Enable colour flag at pos=1

	// Unbind the VAO and VBO so that they are not accidentically used later
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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
	regenerateVBO(); 
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
	regenerateVBO();
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
