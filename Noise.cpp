#include "Noise.hpp"

#include <iostream>
#include <vector>
#include <cmath>

Noise::Noise() {}

Noise::~Noise() {}

const float Noise::grad2D[8][2] {
    {1,0}, {-1,0}, {0,1}, {0,-1},
    {1,1}, {-1,1}, {1,-1}, {-1,-1}

};

int Noise::p[512] = {

    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7,
    225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247,
    120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134,
    139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220,
    105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80,
    73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38,
    147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189,
    28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101,
    155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232,
    178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12,
    191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181,
    199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236,
    205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180

};

float Noise::lerp(float a, float b, float x) {
    return a + x * (b - a);
};

float Noise::grad(int xi, int yi, float x, float y) {
    
        int gIndex = p[(xi + p[yi & 255]) & 255] & 7;

        float gx = grad2D[gIndex][0];
        float gy = grad2D[gIndex][1];

        float dx = x - xi;
        float dy = y - yi;

        return gx * dx + gy * dy;
};

float Noise::fade(float t) {
    return t * t * t * t * (t * (t * 6 - 15) + 10);
}

float Noise::perlin(float x, float y) {

    // determine coordinates of unit square
    int x0 = int(floor(x));
    int x1 = x0 + 1;

    int y0 = int(floor(y));
    int y1 = y0 + 1;

    float sx = fade(x - x0);
    float sy = fade(y - y0);

    float n0 = grad(x0, y0, x, y);
    float n1 = grad(x1, y0, x, y);
    float ix0 = lerp(n0, n1, sx);

    n0 = grad(x0, y1, x, y);
    n1 = grad(x1, y1, x, y);
    float ix1 = lerp(n0, n1, sx);

    return lerp(ix0, ix1, sy);


};

std::vector<std::vector<float>> Noise::GenerateNoiseMap(int mapWidth, int mapHeight) {
    
    std::vector<std::vector<float>> elevation(mapHeight, std::vector<float>(mapWidth));

    // Loop through every position on map
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {

            // Get a number between 0-1
            float nx = float(x)/mapWidth;
            float ny = float(y)/mapHeight;

            elevation[y][x] = perlin(nx, ny);

        }
    }

    return elevation;
};


void Noise::PrintMap(int mapWidth, int mapHeight, std::vector<std::vector<float>> elevation) {

    for (int i = 0; i < mapWidth; i++) {
        for (int j = 0; j < mapHeight; j++) {

            std::cout << elevation[i][j];
        }
        
        std::cout << std::endl;
    }

};
