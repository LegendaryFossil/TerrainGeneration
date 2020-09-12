#pragma once
#include <vector>

#include "glm/glm.hpp"


using FalloffMap = std::vector<glm::vec3>;

FalloffMap generateFalloffMap(const int mapSize);