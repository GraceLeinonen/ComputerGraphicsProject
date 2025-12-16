#pragma once

#include <vector>
#include <glad/glad.h>

class Crosshair {
public:
	Crosshair();

	void draw(GLuint shader); // Draws the crosshair, this should use the screen-space shader
	void setSize(float size);
	float getSize();

private:
	GLuint vao;
	GLuint vbo;
	float size;

	void regenerateVBO();
};
