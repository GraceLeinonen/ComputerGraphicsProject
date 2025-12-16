#pragma once

#include <vector>

/// This class represents Perlin Noise with a certain scale and seed.
/// This class can be used to sample noise at any point x, z, which first get scaled by the set scale factor.
///
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
