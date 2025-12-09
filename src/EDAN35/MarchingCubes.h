#pragma once

#include "TerrainGrid.h"

#include <glm/glm.hpp>
#include <vector>

struct Cube;

class MarchingCubes {
public:
std::pair<GLuint, GLuint> generateMeshVBO(std::vector<std::vector<std::vector<float>>>& densityField, int size_x, int size_y, int size_z, float isoLevel, size_t& outVertexCount);

private:

// edge and triangle tables
static int edgeTable[256];
static int triTable[256][16];

// helper functions
glm::vec3 vertexInterpolation(glm::vec3& p1, glm::vec3& p2, float valp1, float valp2, float isoLevel); // determines point along a voxel cube edge where the isosurface intersects
void processCube(Cube& cube, float isoLevel, std::vector<float>& vertexBuffer); // does the check for individual cubes in grid


};