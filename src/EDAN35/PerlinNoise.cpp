#include "PerlinNoise.h"

#include <random>
#include <algorithm>
#include "core/Bonobo.h"

static const int grad2[8][2] = {
    {1,0}, {-1,0}, {0,1}, {0,-1},
    {1,1}, {-1,1}, {1,-1}, {-1,-1}
};

PerlinNoise::PerlinNoise(int seed, float scale):
	scale(scale), seed(seed)
{

    // permutation table
    std::vector<int> permutation(256);
    for (int i = 0; i < 256; i++) permutation[i] = i;

    std::mt19937 generator(seed); // random numbers which decide how to swap elements
    std::shuffle(permutation.begin(), permutation.end(), generator);

    p.resize(512); // final table has 512 elements
    for (int i = 0; i < 256; i++) {
        p[i] = permutation[i];
        p[i + 256] = permutation[i];
    }
};

int PerlinNoise::getSeed() const {
	return seed;
}

float PerlinNoise::getScale() const {
	return scale;
}

float PerlinNoise::lerp(float a, float b, float t) {

    return a + t * (b - a);

};

// pass in hash, dist in x from point to corner, dist in y from point to corner
float PerlinNoise::grad(int hash, float x, float z) {

    // 8 possible gradient directions
    int h = hash & 7;

    // choose randomly
    float gx = grad2[h][0];
    float gz = grad2[h][1];

    // find dot product
    return gx * x + gz * z;

};

float PerlinNoise::fade(float t) {

    return ((6*t - 15)*t + 10)*t*t*t;

};

float PerlinNoise::sampleNoise(int x, int z) {
	float scaled_x = x * scale;
	float scaled_z = z * scale;

    // determine coordinates of unit cube (and wrap for indexing)
    int x0 = int(floor(scaled_x)) & 255;
    int x1 = (x0 + 1) & 255;
    int z0 = int(floor(scaled_z)) & 255;
    int z1 = (z0 + 1) & 255;

    // determine relative position
    float xf = scaled_x - floor(scaled_x);
    float zf = scaled_z - floor(scaled_z);

    // calculate fade for linear interpolation
    float u = fade(xf);
    float v = fade(zf);

    // distances from point to corners
    float xf0 = xf;
    float xf1 = xf0 - 1;
    float zf0 = zf;
    float zf1 = zf0 - 1;

    // hash values for each corner
    int h00 = p[(p[x0]+z0) & 255];
    int h10 = p[(p[x1]+z0) & 255];
    int h01 = p[(p[x0]+z1) & 255];
    int h11 = p[(p[x1]+z1) & 255];

    // interpolation
    float ix1 = lerp(grad(h00, xf0, zf0), grad(h10, xf1, zf0), u);
    float ix2 = lerp(grad(h01, xf0, zf1), grad(h11, xf1, zf1), u);

	float sample = lerp(ix1, ix2, v);
    return (sample + 1) / 2; // Convert the sample to 0-1
};