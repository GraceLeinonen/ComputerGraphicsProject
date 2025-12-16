#pragma once

#include "TerrainGrid.h"

#include <glm/glm.hpp>
#include <vector>

class DebugPointsRenderer {
public:
	DebugPointsRenderer(TerrainGrid* grid);

	void draw(FPSCameraf* camera, GLuint shader, float pointSize);
	void setDebugPointsRange(glm::ivec3 minIndexes, glm::ivec3 maxIndexes); // Sets a range of indices to draw when using DebugPoints


private:
	GLuint vbo, vao;
	void updateVBO();

	TerrainGrid* grid;
	size_t vertexCount;
	glm::ivec3 minRange, maxRange;
};
