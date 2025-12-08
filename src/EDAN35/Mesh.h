#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Mesh{

    std::vector<glm::vec3> vertices;
    std::vector<int> indices;
    std::vector<glm::vec3> normals;
};