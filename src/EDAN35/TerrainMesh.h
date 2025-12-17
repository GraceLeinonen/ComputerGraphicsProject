#pragma once

#include "TerrainGrid.h"

#include <glm/glm.hpp>
#include <vector>


/// Renderer class that generates and draws the marching cubes mesh for a given TerrainGrid.
/// 
class TerrainMesh {
public:
	TerrainMesh(TerrainGrid* grid);

	void draw(FPSCameraf* camera, GLuint shader, float max_y);
	void setIsoLevel(float iso);
	float getIsoLevel() const;

private:
	GLuint vbo, vao;
	void updateVBO();

	TerrainGrid* grid;
	size_t vertexCount;
	float isoLevel;

	// Marching cube helpers
	static int edgeTable[256];
	static int triTable[256][16];
	glm::vec3 vertexInterpolation(glm::vec3& p1, glm::vec3& p2, float valp1, float valp2);

};
