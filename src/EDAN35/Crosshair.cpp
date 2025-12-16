#include "Crosshair.h"
#include "core/Bonobo.h"

Crosshair::Crosshair() : size(4.0f)
{
	regenerateVBO();
}

void Crosshair::setSize(float newSize) {
	if (newSize == size) return;

	size = newSize;
	regenerateVBO();
}

float Crosshair::getSize() {
	return size;
}

void Crosshair::draw(GLuint shader) {
	glUseProgram(shader); // This should use the screen space shader

	glBindVertexArray(vao);
	glPointSize(size);

	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Crosshair::regenerateVBO() {
	// Delete the old VAO/VBO
	glBindVertexArray(0);
	glUseProgram(0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	// Save the point and generate a new VAO/VBO
	std::vector<float> points;
	points.push_back(0.0f);
	points.push_back(0.0f);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	// Bind the right buffers
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Add the data to the buffers
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0); // the screen position data (at pos=0) needs 2 floats of data
	glEnableVertexAttribArray(0); // Enable position at pos=0

	// Unbind the buffers (so that the next code doesn't accidentically override it)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
