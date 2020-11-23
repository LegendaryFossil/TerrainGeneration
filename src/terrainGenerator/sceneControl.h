#pragma once

#include "glm/glm.hpp"

class Camera;
struct LightData;
struct Mesh;

struct ControlInputData {
  glm::vec2 previousMousePosition;
  bool keyState[256] = {false};
  bool firstInput = true;
};

void handleCameraInput(Camera *camera, const ControlInputData &controlInputData, const double frameTime);
void handleLightInput(Mesh* lightMesh, glm::vec4 *lightPosition, const ControlInputData &controlInputData, const double frameTime);

