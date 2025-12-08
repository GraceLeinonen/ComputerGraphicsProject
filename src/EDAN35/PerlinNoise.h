#pragma once

#include <vector>

class PerlinNoise {
public:
	PerlinNoise(int seed, float scale);
	float sampleNoise(int x, int z); // Returns a value 0-1 of the noise at that position. First X and Z get scaled by the scale factor

	float getScale() const;
	int getSeed() const;
private:
	std::vector<int> p; // permutation table
	float scale;
	float seed;
	float lerp(float a, float b, float t); // helper functions
	float grad(int hash, float x, float z);
	float fade(float t);
};
