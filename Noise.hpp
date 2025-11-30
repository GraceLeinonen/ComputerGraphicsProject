#pragma once

#include <vector>

class Noise {

    private:    
    float perlin(float x, float y);
    float lerp(float a, float b, float x);
    float grad(int ix, int iy, float x, float y);
    float fade(float t);

    static int p[512]; // Perlin's permutation table
    static const float grad2D[8][2];

    public:
    Noise();
    ~Noise();

    static void initPermutation();
    static float perlinNoise2D(float x, float y);

    std::vector<std::vector<float>> GenerateNoiseMap(int mapWidth, int mapHeight);
    void PrintMap(int mapWidth, int mapHeight, std::vector<std::vector<float>> elevation);

};