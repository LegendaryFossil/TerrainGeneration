#include "sceneControl.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "camera.h"
#include "glm\glm.hpp"
#include "lightDefs.h"
#include "meshGenerator.h"

constexpr auto baseSpeed = 100.0f;

void handleCameraInput(Camera *camera, const ControlInputData &controlInputData, const double frameTime) {
  GLfloat cameraSpeed = baseSpeed * float(frameTime);

  GLfloat radian = 0.017453293f; // One radian

  if (controlInputData.keyState[GLFW_KEY_W]) {
    camera->moveForward(cameraSpeed);
  } else if (controlInputData.keyState[GLFW_KEY_S]) {
    camera->moveBackward(cameraSpeed);
  } else if (controlInputData.keyState[GLFW_KEY_A]) {
    camera->moveLeft(cameraSpeed);
  } else if (controlInputData.keyState[GLFW_KEY_D]) {
    camera->moveRight(cameraSpeed);
  } else if (controlInputData.keyState[GLFW_KEY_Q]) {
    camera->moveUp(cameraSpeed);
  } else if (controlInputData.keyState[GLFW_KEY_E]) {
    camera->moveDown(cameraSpeed);
  } else if (controlInputData.keyState[GLFW_KEY_J]) {
    camera->yawRotation(-radian);
  } else if (controlInputData.keyState[GLFW_KEY_L]) {
    camera->yawRotation(radian);
  } else if (controlInputData.keyState[GLFW_KEY_I]) {
    camera->pitchRotation(radian);
  } else if (controlInputData.keyState[GLFW_KEY_K]) {
    camera->pitchRotation(-radian);
  }
}

void handleLightInput(Mesh *lightMesh, LightData *lightData, const ControlInputData &controlInputData,
                      const double frameTime) {
  GLfloat lightSpeed = baseSpeed * float(frameTime);

  if (controlInputData.keyState[GLFW_KEY_W]) {
    lightData->worldLightPosition.z -= lightSpeed;
  } else if (controlInputData.keyState[GLFW_KEY_S]) {
    lightData->worldLightPosition.z += lightSpeed;
  } else if (controlInputData.keyState[GLFW_KEY_A]) {
    lightData->worldLightPosition.x -= lightSpeed;
  } else if (controlInputData.keyState[GLFW_KEY_D]) {
    lightData->worldLightPosition.x += lightSpeed;
  } else if (controlInputData.keyState[GLFW_KEY_Q]) {
    lightData->worldLightPosition.y += lightSpeed;
  } else if (controlInputData.keyState[GLFW_KEY_E]) {
    lightData->worldLightPosition.y -= lightSpeed;
  }

  lightMesh->modelTransformation =
      glm::translate(glm::identity<glm::mat4>(), glm::vec3(lightData->worldLightPosition));
  lightMesh->modelTransformation = glm::scale(lightMesh->modelTransformation, glm::vec3(5.0f));
}