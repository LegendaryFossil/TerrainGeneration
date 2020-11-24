#include "sceneControl.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "camera.h"
#include "glm\glm.hpp"
#include "lightDefs.h"
#include "meshGenerator.h"

constexpr auto walkSpeed = 100.0f;
//constexpr auto keyLookAroundSpeed = 10.0f;
constexpr auto radian = 0.017453293f;

void handleCameraInput(Camera *camera, const ControlInputData &controlInputData, const double frameTime) {
  GLfloat cameraWalkSpeed = walkSpeed * float(frameTime);
  // const auto cameraLookAroundSpeed = radian * keyLookAroundSpeed * float(frameTime);

  if (controlInputData.keyState[GLFW_KEY_W]) {
    camera->moveForward(cameraWalkSpeed);
  } else if (controlInputData.keyState[GLFW_KEY_S]) {
    camera->moveBackward(cameraWalkSpeed);
  } else if (controlInputData.keyState[GLFW_KEY_A]) {
    camera->moveLeft(cameraWalkSpeed);
  } else if (controlInputData.keyState[GLFW_KEY_D]) {
    camera->moveRight(cameraWalkSpeed);
  } else if (controlInputData.keyState[GLFW_KEY_Q]) {
    camera->moveUp(cameraWalkSpeed);
  } else if (controlInputData.keyState[GLFW_KEY_E]) {
    camera->moveDown(cameraWalkSpeed);
  } /*else if (controlInputData.keyState[GLFW_KEY_J]) {
    camera->yawRotation(-radian);
  } else if (controlInputData.keyState[GLFW_KEY_L]) {
    camera->yawRotation(radian);
  } else if (controlInputData.keyState[GLFW_KEY_I]) {
    camera->pitchRotation(radian);
  } else if (controlInputData.keyState[GLFW_KEY_K]) {
    camera->pitchRotation(-radian);
  }*/
}

void handleLightInput(Mesh *lightMesh, glm::vec4 *lightPosition, const ControlInputData &controlInputData,
                      const double frameTime) {
  GLfloat lightSpeed = walkSpeed * float(frameTime);

  if (controlInputData.keyState[GLFW_KEY_W]) {
    lightPosition->z -= lightSpeed;
  } else if (controlInputData.keyState[GLFW_KEY_S]) {
    lightPosition->z += lightSpeed;
  } else if (controlInputData.keyState[GLFW_KEY_A]) {
    lightPosition->x -= lightSpeed;
  } else if (controlInputData.keyState[GLFW_KEY_D]) {
    lightPosition->x += lightSpeed;
  } else if (controlInputData.keyState[GLFW_KEY_Q]) {
    lightPosition->y += lightSpeed;
  } else if (controlInputData.keyState[GLFW_KEY_E]) {
    lightPosition->y -= lightSpeed;
  }

  lightMesh->modelTransformation = glm::translate(glm::identity<glm::mat4>(), glm::vec3(*lightPosition));
  lightMesh->modelTransformation = glm::scale(lightMesh->modelTransformation, glm::vec3(5.0f));
}