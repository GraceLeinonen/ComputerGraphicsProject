#include "SculptingRaycaster.h"
#include "core/Bonobo.h"
#include <glm/gtc/type_ptr.hpp>

SculptingRaycaster::SculptingRaycaster(TerrainGrid* terrain)
	: terrain(terrain), debug_lines_vbo(0), debug_lines_vao(0)
{
	updateVBO(false, false, glm::vec3(0), glm::vec3(0));
}

bool SculptingRaycaster::cast(FPSCameraf* camera, bool destructive, float size, float strength) {
	// Construct the ray
	glm::vec3 origin = camera->mWorld.GetTranslation(); // Get the camera position as origin
	glm::vec3 direction = camera->mWorld.GetFront();

	// Cast the ray
	bool hit = false;
	glm::vec3 rayPos = origin / terrain->getScale(); // Divide by the scale to get to array-index space
	for (int i = 0; i < 1000; i++) {
		// Check if the closest voxel to the ray is in the terrain
		glm::vec3 closestVoxel = glm::round(rayPos);
		// Check if the voxel is part of the TerrainGrid (if it is out of bound, the get() function returns false)
		if (destructive && (terrain->get(closestVoxel) > 0)) {
			// Sculpt the terrain
			terrain->sculpt(closestVoxel, camera, size, strength, destructive);
			hit = true;
			break; // Break from the loop, as we have now sculpted the terrain
		}
		if (!destructive && (terrain->get(closestVoxel) > 0.7)) {
			// Sculpt the terrain
			terrain->sculpt(closestVoxel, camera, size, strength, destructive);
			hit = true;
			break; // Break from the loop, as we have now sculpted the terrain
		}
		// Move the ray forward
		rayPos += direction;
	}

	// Update the VBO for drawing the debug line
	updateVBO(hit, destructive, origin, rayPos * terrain->getScale());
	return hit;
}


void SculptingRaycaster::drawRays(FPSCameraf* camera, GLuint shader) {
	if (debug_lines_vao == 0) {
		return;
	}

	glUseProgram(shader); // Use the simple shader
	// Provide the projection matrix to the shader
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(camera->GetWorldToClipMatrix()));

	glBindVertexArray(debug_lines_vao);

	// Draw the points as a line
	glLineWidth(1.0f);
	glDrawArrays(GL_LINES, 0, 4);

	// Draw the points at the end
	glPointSize(13.0f);
	glDrawArrays(GL_POINTS, 0, 1);

	glBindVertexArray(0);
	glUseProgram(0);
}


void SculptingRaycaster::updateVBO(bool rayHit, bool rayDestructive, glm::vec3 scaledOrigin, glm::vec3 scaledHitPoint) {
	if (debug_lines_vbo == 0 || debug_lines_vao == 0) {
		// If the VBO/VAO are not initialised, create them
		glGenVertexArrays(1, &debug_lines_vao);
		glGenBuffers(1, &debug_lines_vbo);

		// Bind these new VAO and VBO
		glBindVertexArray(debug_lines_vao);
		glBindBuffer(GL_ARRAY_BUFFER, debug_lines_vbo);

		// Setup the attributes in the VAO
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // the world position data (at pos=0) needs 3 floats of data
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // the colour data (at pos=1) needs 3 floats of data
		glEnableVertexAttribArray(0); // Enable position at pos=0
		glEnableVertexAttribArray(1); // Enable colour flag at pos=1
	}
	// Bind the VAO and VBO
	glBindVertexArray(debug_lines_vao);
	glBindBuffer(GL_ARRAY_BUFFER, debug_lines_vbo);


	// Find the colour this ray should be
	glm::vec3 colour;
	if (rayHit && !rayDestructive) {
		colour = glm::vec3(0.1f, 1.0f, 0.1f); // Green if a non-destructive ray hit
	}
	else if (rayHit) {
		colour = glm::vec3(1.0f, 0.1f, 0.1f); // Red if a destructive ray hit
	}
	else {
		colour = glm::vec3(0.1f, 0.1f, 1.0f); // Blue if no hit
	}

	glm::vec3 origin = scaledOrigin / terrain->getScale();
	glm::vec3 hitPoint = scaledHitPoint / terrain->getScale();

	// Add the point and colour data to a list
	std::vector<float> vboData;
	// Add the ray at the scaled positions
	vboData.push_back(scaledHitPoint.x); // add the hitpoint first so we can render this one seperately as a point
	vboData.push_back(scaledHitPoint.y);
	vboData.push_back(scaledHitPoint.z);
	vboData.push_back(colour.x); vboData.push_back(colour.y); vboData.push_back(colour.z);

	vboData.push_back(scaledOrigin.x); vboData.push_back(scaledOrigin.y); vboData.push_back(scaledOrigin.z);
	vboData.push_back(colour.x); vboData.push_back(colour.y); vboData.push_back(colour.z);

	// Add the ray at scale = 1.0
	vboData.push_back(hitPoint.x); vboData.push_back(hitPoint.y); vboData.push_back(hitPoint.z);
	vboData.push_back(1); vboData.push_back(1); vboData.push_back(1);

	vboData.push_back(origin.x); vboData.push_back(origin.y); vboData.push_back(origin.z);
	vboData.push_back(1); vboData.push_back(1); vboData.push_back(1);

	// Fill the VBO with the point and colour data
	glBufferData(GL_ARRAY_BUFFER, vboData.size() * sizeof(float), vboData.data(), GL_STATIC_DRAW);



	// Unbind the buffers (so that the next code doesn't accidentically override it)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
