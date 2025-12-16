#pragma once

#include "TerrainGrid.h"
#include <vector>
#include "core/FPSCamera.h"
#include <glm/vec3.hpp>

class SculptingRaycaster {
public:
	SculptingRaycaster() = delete;
	SculptingRaycaster(TerrainGrid* terrain);
	// Casts a sculpting ray from a mouse position. Returns true if terrain was hit.
	// if destructive = true, terrain is removed, otherwise terrain is added
	// size is the radius of the sculpting brush
	bool cast(FPSCameraf* camera,  bool desctructive, float size); 

	void drawRays(FPSCameraf* camera, GLuint shader); // Draws a debug line for the rays 

private:
	TerrainGrid* terrain; // The terrain to be sculpted

	GLuint debug_lines_vao; // VAO & VBO for drawing a debug line for the last ray
	GLuint debug_lines_vbo; 

	void updateVBO(bool rayHit, bool rayDestructive, glm::vec3 scaledOrigin, glm::vec3 scaledHitPoint); // Given information about the ray, rebuild the VBO for drawing the debug line

};
