#include "camera.h"

Camera::Camera()
    : _cameraPositionCartesian(0.0f, 0.0f, 0.0f),
      _cameraTargetSpherical(1.0f, glm::half_pi<float>(), -glm::half_pi<float>()),
      _viewMatrixAxes(glm::mat4(1.0f)), _camPosTranslation(glm::mat4(1.0f)) {}

Camera::Camera(const glm::vec3 &cameraPositionCartesian, const glm::vec3 &cameraTargetSpherical)
    : _cameraPositionCartesian(cameraPositionCartesian),
      _cameraTargetSpherical(cameraTargetSpherical),
      _viewMatrixAxes(glm::mat4(1.0f)), _camPosTranslation(glm::mat4(1.0f)) {}

// View matrix calculated according to http://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml
glm::mat4 Camera::createViewMatrix() {
  // We add a direction vector (the camera target) to the camera position so the "real" target
  // becomes dependent on the position (the target will always be in front of the position).
  // This way, whenever the camera moves (x,y,z) units the target will move the same amount.
  glm::vec3 lookDirection =
      glm::normalize(_cameraPositionCartesian + sphericalToCartesian(_cameraTargetSpherical) -
                     _cameraPositionCartesian);
  static glm::vec3 upDirection = glm::normalize(
      glm::vec3(0.0f, 1.0f, 0.0f)); // Up direction of camera, aligned with world's y-axis at first.
  glm::vec3 rightDirection = glm::normalize(
      glm::cross(lookDirection, upDirection)); // Calculate remaining direction of camera
  glm::vec3 perpUpDirection =
      glm::cross(rightDirection,
                 lookDirection); // Re-calculate up direction, basis vectors may not be orthonormal

  // Create view matrix, [0] is first column, [1] is second..., [0][1] would be first column, second
  // row etc.
  _viewMatrixAxes[0] = glm::vec4(rightDirection, 0.0f);
  _viewMatrixAxes[1] = glm::vec4(perpUpDirection, 0.0f);
  _viewMatrixAxes[2] = glm::vec4(-lookDirection, 0.0f);

  _viewMatrixAxes = glm::transpose(_viewMatrixAxes);

  _camPosTranslation[3] = glm::vec4(-_cameraPositionCartesian, 1.0f);

  _viewMatrix = _viewMatrixAxes * _camPosTranslation;

  return _viewMatrix;
}

void Camera::yawRotation(GLfloat radian) { _cameraTargetSpherical.y += -radian; }

void Camera::pitchRotation(GLfloat radian) {
  _cameraTargetSpherical.z += radian;

  _cameraTargetSpherical.z =
      glm::clamp(_cameraTargetSpherical.z, -glm::pi<float>() + 0.00001f, -0.00001f);
}

// The third row of the view matrix is the axis of the camera pointing
// in its look direction. Add/subtract to move camera in that direction
void Camera::moveForward(GLfloat moveSpeed) {
  _cameraPositionCartesian.x -= (_viewMatrix[0][2] * moveSpeed);
  _cameraPositionCartesian.y -= (_viewMatrix[1][2] * moveSpeed);
  _cameraPositionCartesian.z -= (_viewMatrix[2][2] * moveSpeed);
}

void Camera::moveBackward(GLfloat moveSpeed) {
  _cameraPositionCartesian.x += (_viewMatrix[0][2] * moveSpeed);
  _cameraPositionCartesian.y += (_viewMatrix[1][2] * moveSpeed);
  _cameraPositionCartesian.z += (_viewMatrix[2][2] * moveSpeed);
}

// The first row of the view matrix is the axis of the camera pointing
// in its right direction. Add/subtract to move camera in that direction
void Camera::moveLeft(GLfloat moveSpeed) {
  _cameraPositionCartesian.x -= (_viewMatrix[0][0] * moveSpeed);
  _cameraPositionCartesian.y -= (_viewMatrix[1][0] * moveSpeed);
  _cameraPositionCartesian.z -= (_viewMatrix[2][0] * moveSpeed);
}

void Camera::moveRight(GLfloat moveSpeed) {
  _cameraPositionCartesian.x += (_viewMatrix[0][0] * moveSpeed);
  _cameraPositionCartesian.y += (_viewMatrix[1][0] * moveSpeed);
  _cameraPositionCartesian.z += (_viewMatrix[2][0] * moveSpeed);
}

// The first row of the view matrix is the axis of the camera pointing
// in its up direction. Add/subtract to move camera in that direction
void Camera::moveUp(GLfloat moveSpeed) {
  _cameraPositionCartesian.x += (_viewMatrix[0][1] * moveSpeed);
  _cameraPositionCartesian.y += (_viewMatrix[1][1] * moveSpeed);
  _cameraPositionCartesian.z += (_viewMatrix[2][1] * moveSpeed);
}

void Camera::moveDown(GLfloat moveSpeed) {
  _cameraPositionCartesian.x -= (_viewMatrix[0][1] * moveSpeed);
  _cameraPositionCartesian.y -= (_viewMatrix[1][1] * moveSpeed);
  _cameraPositionCartesian.z -= (_viewMatrix[2][1] * moveSpeed);
}

// Spherical-to-cartesian relation: http://mathworld.wolfram.com/SphericalCoordinates.html
glm::vec3 Camera::sphericalToCartesian(glm::vec3 sphericalCoordinate) {
  glm::vec3 cartesianCoordinate;

  cartesianCoordinate.x =
      sphericalCoordinate.x * glm::cos(sphericalCoordinate.y) * glm::sin(sphericalCoordinate.z);
  cartesianCoordinate.y = sphericalCoordinate.x * glm::cos(sphericalCoordinate.z);
  cartesianCoordinate.z =
      sphericalCoordinate.x * glm::sin(sphericalCoordinate.y) * glm::sin(sphericalCoordinate.z);

  return cartesianCoordinate;
}