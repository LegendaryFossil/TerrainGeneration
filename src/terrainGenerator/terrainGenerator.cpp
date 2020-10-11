#include <unordered_map>

#include "GL/glew.h"
#include "sceneUI.h"

#include "GLFW/glfw3.h" // Include this header last always to avoid conflicts with loading new OpenGL versions

#include "camera.h"
#include "falloffMapGenerator.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "lightDefs.h"
#include "meshGenerator.h"
#include "noiseMapGenerator.h"
#include "sceneControl.h"
#include "sceneDefs.h"
#include "sceneRendering.h"
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
SceneProgramObjects sceneProgramObjects;
SceneSettings sceneSettings = {};

static void errorCallback(int error, const char *description) { fprintf(stderr, "Error: %s\n", description); }

static void frameBufferSizeCallBack(GLFWwindow *window, int width, int height) {
  // When minimizing
  if (width == 0 || height == 0) {
    return;
  }

  glViewport(0, 0, width, height);
  setUniform(sceneProgramObjects.at(kTerrainGeneratorProgramObjectName), ufViewportSizeName,
             glm::vec2(width, height));
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
  glGenFramebuffers(1, &sceneData.sceneFrameBuffer.fboHandle);
  glBindFramebuffer(GL_FRAMEBUFFER, sceneData.sceneFrameBuffer.fboHandle);

  glGenTextures(1, &sceneData.sceneFrameBuffer.fboTexture);
  createTexture2D(&sceneData.sceneFrameBuffer.fboTexture, GL_CLAMP_TO_EDGE, GL_NEAREST, windowData.width,
                  windowData.height, GL_FLOAT, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         sceneData.sceneFrameBuffer.fboTexture, 0);

  glGenRenderbuffers(1, &sceneData.sceneFrameBuffer.rboHandle);
  glBindRenderbuffer(GL_RENDERBUFFER, sceneData.sceneFrameBuffer.rboHandle);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowData.width, windowData.height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                            sceneData.sceneFrameBuffer.rboHandle);

  const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    assert(false);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initSceneData() {
  sceneData.terrainData = getDefaultTerrainData();
  sceneData.meshIdToMesh = initSceneMeshes(sceneData.terrainData);
  sceneProgramObjects = initSceneShaders(windowData, sceneData);
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

void renderScene() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  handleSceneUiInput(&sceneSettings, &sceneData.terrainData, &sceneData.meshIdToMesh, sceneProgramObjects);

  glfwPollEvents();

  if (sceneSettings.controlMode == SceneSettings::CONTROL_MODE::CAMERA) {
    handleCameraInput(&sceneData.fpsCamera, controlInputData, frameTimeData.frameTime);
  } else if (sceneSettings.controlMode == SceneSettings::CONTROL_MODE::LIGHT) {
    handleLightInput(&sceneData.lightData, controlInputData, frameTimeData.frameTime);
  }

  // Measure time each frame
  updateFrameTime(&frameTimeData);

  const auto viewToClipMatrix = glm::perspective(
      sceneData.viewFrustumData.fieldOfView, float(windowData.width) / float(windowData.height),
      sceneData.viewFrustumData.nearPlane, sceneData.viewFrustumData.farPlane);
  switch (sceneSettings.renderMode) {
  case SceneSettings::RENDER_MODE::NOISE_MAP:
    renderNoiseMap(sceneData.meshIdToMesh.at(kTerrainMeshId), sceneData.fpsCamera.createViewMatrix(),
                   viewToClipMatrix, sceneProgramObjects.at(kTerrainGeneratorDebugProgramObjectName));
    break;
  case SceneSettings::RENDER_MODE::COLOR_MAP:
    renderColorMap(sceneData.meshIdToMesh.at(kTerrainMeshId), sceneData.fpsCamera.createViewMatrix(),
                   viewToClipMatrix, sceneProgramObjects.at(kTerrainGeneratorDebugProgramObjectName));
    break;
  case SceneSettings::RENDER_MODE::FALLOFF_MAP:
    renderFalloffMap(sceneData.meshIdToMesh.at(kTerrainMeshId), sceneData.fpsCamera.createViewMatrix(),
                     viewToClipMatrix, sceneProgramObjects.at(kTerrainGeneratorDebugProgramObjectName));
    break;
  case SceneSettings::RENDER_MODE::MESH: {
    renderTerrain(&sceneData, frameTimeData.frameTime, viewToClipMatrix, false,
                  sceneProgramObjects.at(kTerrainGeneratorProgramObjectName));
    /*renderQuad(sceneData.meshIdToMesh.at(kQuadMeshId), sceneData.sceneFrameBuffer.fboTexture,
               sceneData.fpsCamera.createViewMatrix(), viewToClipMatrix,
               sceneProgramObjects.at(kQuadShaderProgramObjectName));
    renderWater(sceneData.meshIdToMesh.at(kWaterMeshId), sceneData.sceneFrameBuffer.fboTexture,
                sceneData.fpsCamera.createViewMatrix(), viewToClipMatrix,
                sceneProgramObjects.at(kWaterProgramObjectName));*/
  } break;
  case SceneSettings::RENDER_MODE::WIREFRAME:
    renderTerrain(&sceneData, frameTimeData.frameTime, viewToClipMatrix, true,
                  sceneProgramObjects.at(kTerrainGeneratorProgramObjectName));
    break;
  default:
    assert(false);
    break;
  }

  // renderWater();

  glfwSwapBuffers(windowData.window);
}

void freeResources() {
  for (auto &[meshId, mesh] : sceneData.meshIdToMesh) {
    glDeleteBuffers(1, &mesh.vboHandle);
    glDeleteBuffers(1, &mesh.iboHandle);

    for (auto &textureHandle : mesh.textureHandles) {
      glDeleteTextures(1, &textureHandle);
    }
  }

  glDeleteBuffers(1, &sceneData.sceneFrameBuffer.fboHandle);

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