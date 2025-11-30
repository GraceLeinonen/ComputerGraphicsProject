#include "Noise.hpp"

int main() {

    Noise noise;
    int mapHeight = 3;
    int mapWidth = 3;

    std::vector<std::vector<float>> map = noise.GenerateNoiseMap(mapWidth, mapHeight);
    noise.PrintMap(mapWidth, mapHeight, map);

    return 0;
}
