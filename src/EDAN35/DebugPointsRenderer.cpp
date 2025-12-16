#include "DebugPointsRenderer.h"
#include <glm/gtc/type_ptr.hpp>
#include "core/Bonobo.h"

DebugPointsRenderer::DebugPointsRenderer(TerrainGrid* grid) {
	this->grid = grid;
	this->vertexCount = 0;

	grid->registerUpdateCallback([this]() { this->updateVBO(); });
	updateVBO();
};


void DebugPointsRenderer::draw(FPSCameraf* camera, GLuint shader, float pointSize) {
	// If there is no VBO yet, create one
	if (vbo == 0) {
		updateVBO();
	}

	glUseProgram(shader); // Use the debug point shader
	// Provide the projection matrix to the shader
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(camera->GetWorldToClipMatrix()));

	glBindVertexArray(vao);
	glPointSize(pointSize);

	glDrawArrays(GL_POINTS, 0, vertexCount);
	glBindVertexArray(0);
	glUseProgram(0);
}


void DebugPointsRenderer::setDebugPointsRange(glm::ivec3 minIndexes, glm::ivec3 maxIndexes) {
		// Ensure the min and max are the min and the max
		glm::ivec3 min = glm::min(minIndexes, maxIndexes);
		glm::ivec3 max = glm::max(minIndexes, maxIndexes);

		// Clamp the min and max to legal values
		min = glm::clamp(min, glm::ivec3(0), grid->get_dimensions());
		max = glm::clamp(max, glm::ivec3(0), grid->get_dimensions());
		int count = (max.x - min.x) * (max.y - min.y) * (max.z - min.z);

		if (min == minRange && max == maxRange && count == vertexCount) {
			// No change, so we dont have to regenerate the VBO
			return;
		}

		// Save the ranges
		minRange = min;
		maxRange= max;

		vertexCount = count;

		// Regenerate the vbo
		updateVBO();
}

void DebugPointsRenderer::updateVBO() {
	// If we still have old debug points, delete them
	if (vbo != 0) {
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	// Generate the new VAO and VBO
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	// Bind the VAO and VBO
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Load all the points into a float array
	std::vector<float> points;
	for (int x = minRange.x; x < maxRange.x; x++)
		for (int y = minRange.y; y < maxRange.y; y++)
			for (int z = minRange.z; z < maxRange.z; z++) {
				points.push_back((float)x * grid->get_scale());
				points.push_back((float)y * grid->get_scale());
				points.push_back((float)z * grid->get_scale());
				if (grid->get(glm::ivec3(x, y, z))) {
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
