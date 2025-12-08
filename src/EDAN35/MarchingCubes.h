#pragma once

#include "Mesh.h"
#include "TerrainGrid.h"

#include <glm/glm.hpp>
#include <vector>

class MarchingCubes {
public:
MarchingCubes();
Mesh GenerateMesh(std::vector<std::vector<std::vector<float>>>& densityField);

private:
// threshold where surface meets air
float isoLevel;

// edge and triangle tables
static int edgeTable[256];
static int triTable[256][16];

// helper functions
int processCube(TerrainGrid grid); // does the check for individual cubes in grid


};