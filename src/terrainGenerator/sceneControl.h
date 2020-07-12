#pragma once

#include "glm/glm.hpp"

class Camera;
struct LightData;

struct ControlInputData {
  glm::dvec2 previousMousePosition;
  bool keyState[256] = {false};
};

void handleCameraInput(Camera *camera, const ControlInputData &controlInputData, const double frameTime);
void handleLightInput(LightData *lightData, const ControlInputData &controlInputData, const double frameTime);

