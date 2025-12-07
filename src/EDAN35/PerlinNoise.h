#pragma once

#include <vector>

class PerlinNoise {
public:
PerlinNoise();
float noise(float x, float z);

private:
std::vector<int> p; // permutation table
float lerp(float a, float b, float t); // helper functions
float grad(int hash, float x, float z);
float fade(float t);

};