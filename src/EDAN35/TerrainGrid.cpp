#include "TerrainGrid.h"
#include "core/Bonobo.h"

TerrainGrid::TerrainGrid(int x, int y, int z, float scale)
	: x_size(x), y_size(y), z_size(z), grid(x * y * z, false), scale(scale) // Set the x, y and z sizes, and initiallise the grid to all empty values
{}

bool TerrainGrid::get(int x, int y, int z) const {
	return grid[x + y * x_size + z * x_size * y_size];
}

void TerrainGrid::set(int x, int y, int z, bool newValue) {
	grid[x + y * x_size + z * x_size * y_size] = newValue;
}

int TerrainGrid::get_x_size() const {
	return x_size;
}

int TerrainGrid::get_y_size() const {
	return y_size;
}

int TerrainGrid::get_z_size() const {
	return z_size;
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


void TerrainGrid::clear() {
	for (int i = 0; i < grid.size(); i++) {
		grid[i] = false;
	}

	for (int x = 0; x < x_size; x++) {
		for (int z = 0; z < z_size; z++) {
			grid[x + z * y_size * x_size] = true;
		}
	}
}

std::pair<GLuint, GLuint> TerrainGrid::debugPointsVBO() {
	LogInfo("Creating a new VAO/VBO for the debug points of the terrain grid.");

	// For each of the points, add them to a float array and add a colour flag
	std::vector<float> points;
	for (int x = 0; x < x_size; x++)
		for (int y = 0; y < y_size; y++)
			for (int z = 0; z < z_size; z++) {
				points.push_back((float)x * scale);
				points.push_back((float)y * scale);
				points.push_back((float)z * scale);
				if (is_solid(x, y, z)) {
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


//! This is just creating an elevation map based on the size of the grid
void TerrainGrid::regenerate() {

	elevationMap.resize(x_size, std::vector<float>(z_size));

	for (int x = 0; x < x_size; x++) {
		for (int z = 0; z < z_size; z++) {
			float n = noise.noise(x * 0.1f, z * 0.1f); //! NOTE: NEED TO SCALE
			elevationMap[x][z] = (n + 1.0) * 0.5 * y_size;
			std::cout << elevationMap[x][n] << " ";
		}
		std::cout << std::endl;
	}

	


}

void TerrainGrid::resize(int newX, int newY, int newZ) {
	std::vector<uint8_t> newGrid(newX * newY * newZ, false);

	// Loop through the overlapping area between the new and old grid, and move the values to the new grid coordinates.
	for (int x = 0; x < std::min(x_size, newX); x++) {
		for (int y = 0; y < std::min(y_size, newY); y++) {
			for (int z = 0; z < std::min(z_size, newZ); z++) {
				newGrid[x + y * newX + z * newX * newY] = get(x, y, z);
			}
		}
	}

	x_size = newX;
	y_size = newY;
	z_size = newZ;
	grid = newGrid;
}


bool TerrainGrid::is_solid(int x, int y, int z) {
	
	// Handle boundary cases
	if (x < 0 || z < 0 || x >= x_size || z >= z_size) return false;

	// check if y is less than value in elevation map
	return y <= elevationMap[x][z];

}
