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

  void yawRotation(GLfloat angle);
  void pitchRotation(GLfloat angle);
  void invertPitch();

  void moveForward(GLfloat moveSpeed);
  void moveBackward(GLfloat moveSpeed);
  void moveLeft(GLfloat moveSpeed);
  void moveRight(GLfloat moveSpeed);
  void moveUp(GLfloat moveSpeed);
  void moveDown(GLfloat moveSpeed);

  void setCameraPosition(const glm::vec3 &cameraPosition) { _cameraPositionCartesian = cameraPosition; }
  glm::vec3 cameraPosition() { return _cameraPositionCartesian; }

private:
  glm::vec3 sphericalToCartesian(glm::vec3 sphericalCoordinate);

  glm::mat4 _viewMatrix;
  glm::mat4 _viewMatrixAxes;
  glm::mat4 _camPosTranslation;

  glm::vec3 _cameraPositionCartesian;
  glm::vec3 _cameraTargetSpherical;
};