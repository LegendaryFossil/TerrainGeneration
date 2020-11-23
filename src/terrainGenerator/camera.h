#pragma once

#include "glm\glm.hpp"
#include "glm\gtc\constants.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"

#include "GL\glew.h"

struct ViewFrustumData {
  float fieldOfView = 45.0f;
  float nearPlane = 0.1f;
  float farPlane = 10000.0f;
};

class Camera {
public:
  Camera();
  Camera(const glm::vec3 &cameraPositionCartesian, const glm::vec3 &cameraTargetSpherical);

  glm::mat4 createViewMatrix();

  void yawRotation(float angle);
  void pitchRotation(float angle);
  void invertPitch();

  void moveForward(float moveSpeed);
  void moveBackward(float moveSpeed);
  void moveLeft(float moveSpeed);
  void moveRight(float moveSpeed);
  void moveUp(float moveSpeed);
  void moveDown(float moveSpeed);

  void setCameraPosition(const glm::vec3 &cameraPosition) { _cameraPositionCartesian = cameraPosition; }
  glm::vec3 cameraPosition() const { return _cameraPositionCartesian; }

private:
  glm::vec3 sphericalToCartesian(glm::vec3 sphericalCoordinate);

  glm::mat4 _viewMatrix;
  glm::mat4 _viewMatrixAxes;
  glm::mat4 _camPosTranslation;

  glm::vec3 _cameraPositionCartesian;
  glm::vec3 _cameraTargetSpherical;
};