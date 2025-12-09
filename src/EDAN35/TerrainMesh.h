#pragma once

#include "TerrainGrid.h"

#include <glm/glm.hpp>
#include <vector>

class TerrainMesh {
public:
TerrainMesh(TerrainGrid& grid, float isoLevel);
~TerrainMesh();

// Marching cube logic
std::pair<GLuint, GLuint> generateMeshVBO();
size_t getVertexCount();

void setTerrainGrid(TerrainGrid& grid);
void setisoLevel(float isoLevel);

private:
TerrainGrid* grid;
float isoLevel;
size_t vertexCount;

// Marching cube helpers
static int edgeTable[256];
static int triTable[256][16];
glm::vec3 vertexInterpolation(glm::vec3& p1, glm::vec3& p2, float valp1, float valp2);


};