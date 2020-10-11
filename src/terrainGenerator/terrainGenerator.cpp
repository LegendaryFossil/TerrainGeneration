#include <unordered_map>

#include "sceneUI.h"

#include "GL/glew.h"

#include "GLFW/glfw3.h" // Include this header last always to avoid conflicts with loading new OpenGL versions

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"

#include "camera.h"
#include "falloffMapGenerator.h"
#include "lightDefs.h"
#include "meshGenerator.h"
#include "noiseMapGenerator.h"
#include "sceneControl.h"
#include "sceneDefs.h"
#include "sceneShaders.h"
#include "shaderLoader.h"
#include "terrainDefs.h"
#include "textureGenerator.h"
#include "timeMeasureUtils.h"
#include "uniformDefs.h"
#include "windowDefs.h"

WindowData windowData = {};
SceneData sceneData = {};
ControlInputData controlInputData = {};
FrameTimeData frameTimeData = {};

MeshIdToMesh meshIdToMesh;

SceneProgramObjects sceneProgramObjects;

SceneSettings sceneSettings = {};

struct ViewFrustumData {
  float fieldOfView = 45.0f;
  float nearPlane = 0.1f;
  float farPlane = 10000.0f;
} viewFrustumData;

unsigned int fboHandle;
unsigned int fboTexture;
unsigned int rboHandle;

static void errorCallback(int error, const char *description) { fprintf(stderr, "Error: %s\n", description); }

static void frameBufferSizeCallBack(GLFWwindow *window, int width, int height) {
  // When minimizing
  if (width == 0 || height == 0) {
    return;
  }

  glViewport(0, 0, width, height);
  setUniform(sceneProgramObjects.at(kTerrainGeneratorProgramObjectName), ufViewportSizeName, glm::vec2(width, height));
}

static void resizeWindowCallback(GLFWwindow *window, int width, int height) {
  // When minimizing
  if (width == 0 || height == 0) {
    return;
  }

  windowData.width = width;
  windowData.height = height;
  windowData.center = glm::vec2(width / 2.0f, height / 2.0f);
}

static void cursorPosCallback(GLFWwindow *window, double xPos, double yPos) {
  /*glm::dvec2 mousePosition(xPos, yPos);
  glm::dvec2 mouseDiff = mousePosition - controlInputData.previousMousePosition;
  controlInputData.previousMousePosition = mousePosition;

  GLfloat mouseSpeed = 0.001f;
  fpsCamera.yawRotation(GLfloat(mouseDiff.x * mouseSpeed));
  fpsCamera.pitchRotation(GLfloat(mouseDiff.y * mouseSpeed));*/
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key > 0 && key < 256) {
    if (action == GLFW_PRESS) {
      controlInputData.keyState[key] = true;
    } else if (action == GLFW_RELEASE) {
      controlInputData.keyState[key] = false;
    }
  }

  if (controlInputData.keyState[GLFW_KEY_M]) {
    sceneSettings.showImGuiDemo = !sceneSettings.showImGuiDemo;
  }

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void initFrameBuffers() {
  glGenFramebuffers(1, &fboHandle);
  glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

  glGenTextures(1, &fboTexture);
  createTexture2D(&fboTexture, GL_CLAMP_TO_EDGE, GL_NEAREST, windowData.width, windowData.height, GL_FLOAT, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

  glGenRenderbuffers(1, &rboHandle);
  glBindRenderbuffer(GL_RENDERBUFFER, rboHandle);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowData.width, windowData.height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboHandle);

  const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    assert(false);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initSceneData() {
  sceneData.terrainData = getDefaultTerrainData();

  // Meshes
  meshIdToMesh = initSceneMeshes(sceneData.terrainData);

  // Shaders
  sceneProgramObjects = initSceneShaders(windowData, sceneData);
  sceneSettings.selectedProgramObject = sceneProgramObjects.at(kTerrainGeneratorProgramObjectName);

  // FBOs
  initFrameBuffers();
}

void initGLStates() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST); // Enable depth testing
  glEnable(GL_CULL_FACE);  // Enable backface-culling
  glFrontFace(GL_CCW);
  glViewport(0, 0, windowData.width, windowData.height);

  // Number of input control points for patch in Tess. Control Shader
  glPatchParameteri(GL_PATCH_VERTICES, 1);
}

void renderTerrain(const glm::mat4 &viewMatrix) {
  const auto &terrainMesh = meshIdToMesh.at(kTerrainMeshId);

  if (sceneSettings.renderMode == SceneSettings::RENDER_MODE::NOISE_MAP) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[0]);
    setUniform(sceneSettings.selectedProgramObject, ufDebugSettings, glm::vec3(1.0f, 0.0f, 0.0f));
  } else if (sceneSettings.renderMode == SceneSettings::RENDER_MODE::COLOR_MAP) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[1]);
    setUniform(sceneSettings.selectedProgramObject, ufDebugSettings, glm::vec3(0.0f, 1.0f, 0.0f));
  } else if (sceneSettings.renderMode == SceneSettings::RENDER_MODE::FALLOFF_MAP) {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[2]);
    setUniform(sceneSettings.selectedProgramObject, ufDebugSettings, glm::vec3(0.0f, 0.0f, 1.0f));
  } else if (sceneSettings.renderMode == SceneSettings::RENDER_MODE::MESH ||
             sceneSettings.renderMode == SceneSettings::RENDER_MODE::WIREFRAME) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[0]); // Height map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[1]); // Color map
  }

  glBindVertexArray(terrainMesh.vaoHandle);
  glUseProgram(sceneSettings.selectedProgramObject);
  {
    setUniform(sceneSettings.selectedProgramObject, ufModelToWorldMatrixName, terrainMesh.modelTransformation);
    setUniform(sceneSettings.selectedProgramObject, ufWorldToViewMatrixName, viewMatrix);
    setUniform(sceneSettings.selectedProgramObject, ufNormalMatrix,
               glm::transpose(glm::inverse(glm::mat3(viewMatrix * terrainMesh.modelTransformation))));

    const auto viewToClipMatrix =
        glm::perspective(viewFrustumData.fieldOfView, float(windowData.width) / float(windowData.height),
                         viewFrustumData.nearPlane, viewFrustumData.farPlane);
    setUniform(sceneSettings.selectedProgramObject, ufViewToClipMatrixName, viewToClipMatrix);

    setUniform(sceneProgramObjects[kTerrainGeneratorProgramObjectName], ufWorldLightName,
               sceneData.lightData.worldLightPosition);

    if (sceneSettings.renderMode == SceneSettings::RENDER_MODE::WIREFRAME) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDrawElements(GL_PATCHES, GLsizei(terrainMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
  }
  glUseProgram(0);
}

void renderReflectionTexture() {
  glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CLIP_DISTANCE1);

  // Move camera under water (twice distance from water to camera)
  // This assumes no model transformation affects the terrain (i.e. identity matrix transformation)
  // and that water height is always at y = 0.0
  const auto waterPositionY = 0.0f;
  auto cameraPosition = sceneData.fpsCamera.cameraPosition();
  const auto distanceToMoveY = 2.0f * (sceneData.fpsCamera.cameraPosition().y - waterPositionY);
  cameraPosition.y -= distanceToMoveY;
  sceneData.fpsCamera.setCameraPosition(cameraPosition);
  sceneData.fpsCamera.invertPitch();

  // Render to texture
  renderTerrain(sceneData.fpsCamera.createViewMatrix());

  // Change camera back to original state
  cameraPosition.y += distanceToMoveY;
  sceneData.fpsCamera.setCameraPosition(cameraPosition);
  sceneData.fpsCamera.invertPitch();

  glDisable(GL_CLIP_DISTANCE1);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderWater() {
  const auto waterProgramObject = sceneProgramObjects[kWaterShaderProgramObjectName];
  const auto &waterMesh = meshIdToMesh.at(kWaterMeshId);

  glBindVertexArray(waterMesh.vaoHandle);
  glUseProgram(waterProgramObject);
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboTexture); // Scene texture

    setUniform(waterProgramObject, ufModelToWorldMatrixName, waterMesh.modelTransformation);

    setUniform(waterProgramObject, ufWorldToViewMatrixName, sceneData.fpsCamera.createViewMatrix());

    setUniform(waterProgramObject, ufViewToClipMatrixName,
               glm::perspective(viewFrustumData.fieldOfView, float(windowData.width) / float(windowData.height),
                                viewFrustumData.nearPlane, viewFrustumData.farPlane));

    glDrawElements(GL_TRIANGLES, GLsizei(waterMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
  }
  glUseProgram(0);
}

void renderQuad() {
  const auto quadProgramObject = sceneProgramObjects.at(kQuadShaderProgramObjectName);
  const auto &quadMesh = meshIdToMesh.at(kQuadMeshId);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, fboTexture);

  glBindVertexArray(quadMesh.vaoHandle);
  glUseProgram(quadProgramObject);
  {
    setUniform(quadProgramObject, ufModelToWorldMatrixName, quadMesh.modelTransformation);

    setUniform(quadProgramObject, ufWorldToViewMatrixName, sceneData.fpsCamera.createViewMatrix());

    setUniform(quadProgramObject, ufViewToClipMatrixName,
               glm::perspective(viewFrustumData.fieldOfView, float(windowData.width) / float(windowData.height),
                                viewFrustumData.nearPlane, viewFrustumData.farPlane));

    glDrawElements(GL_TRIANGLES, GLsizei(quadMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
  }
  glUseProgram(0);
}

void renderScene() {
  glfwPollEvents();

  if (sceneSettings.controlMode == SceneSettings::CONTROL_MODE::CAMERA) {
    handleCameraInput(&sceneData.fpsCamera, controlInputData, frameTimeData.frameTime);
  } else if (sceneSettings.controlMode == SceneSettings::CONTROL_MODE::LIGHT) {
    handleLightInput(&sceneData.lightData, controlInputData, frameTimeData.frameTime);
  }

  // Measure time each frame
  updateFrameTime(&frameTimeData);

  renderReflectionTexture();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, fboTexture); // Scene texture
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, meshIdToMesh.at(kTerrainMeshId).textureHandles[3]);
  sceneData.terrainData.waterDistortionMoveFactor +=
      sceneData.terrainData.waterDistortionSpeed * float(frameTimeData.frameTime);
  sceneData.terrainData.waterDistortionMoveFactor = std::fmod(sceneData.terrainData.waterDistortionMoveFactor, 1.0f);

  setUniform(sceneSettings.selectedProgramObject, ufWaterDistortionMoveFactorName,
             sceneData.terrainData.waterDistortionMoveFactor);
  renderTerrain(sceneData.fpsCamera.createViewMatrix());

  // renderQuad();

  // renderWater();

  handleSceneUiInput(&sceneSettings, &sceneData.terrainData, &meshIdToMesh, sceneProgramObjects);
  glfwSwapBuffers(windowData.window);
}

void freeResources() {
  for (auto &[meshId, mesh] : meshIdToMesh) {
    glDeleteBuffers(1, &mesh.vboHandle);
    glDeleteBuffers(1, &mesh.iboHandle);

    for (auto &textureHandle : mesh.textureHandles) {
      glDeleteTextures(1, &textureHandle);
    }
  }

  glDeleteBuffers(1, &fboHandle);

  for (const auto &[programObjectName, programObject] : sceneProgramObjects) {
    deleteProgramObject(programObject);
  }

  destroyUI();

  glfwDestroyWindow(windowData.window);
  glfwTerminate();
}

void initTerrainGenerator() {
  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    glfwTerminate();
    assert(false);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  windowData.window = glfwCreateWindow(windowData.width, windowData.height, "Terrain Generator", NULL, NULL);
  if (!windowData.window) {
    glfwTerminate();
    assert(false);
  }

  glfwSetFramebufferSizeCallback(windowData.window, frameBufferSizeCallBack);
  glfwSetWindowSizeCallback(windowData.window, resizeWindowCallback);

  glfwSetKeyCallback(windowData.window, keyCallback);

  // Disable mouse movements for now...
  // glfwSetCursorPosCallback(windowData.window, cursorPosCallback);
  glfwSetCursorPos(windowData.window, windowData.center.x, windowData.center.y);
  controlInputData.previousMousePosition = windowData.center;

  glfwMakeContextCurrent(windowData.window);

  GLenum glewRes = glewInit();
  if (glewRes != GLEW_OK) {
    fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(glewRes));
    glfwTerminate();
    assert(false);
  }

  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *version = glGetString(GL_VERSION);
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  // glfwSwapInterval(1); // Gives input latency

  initUI(windowData.window, "#version 130");

  initGLStates();
  initSceneData();

  while (!glfwWindowShouldClose(windowData.window)) {
    renderScene();
  }

  freeResources();
}

int main(int argc, char **argv) {
  initTerrainGenerator();
  return 0;
}