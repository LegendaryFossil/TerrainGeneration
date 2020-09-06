#include <array>

#include "imGui/imgui.h"
#include "imGui/imgui_impl_glfw.h"
#include "imGui/imgui_impl_opengl3.h"

#include "GL/glew.h"

#include "GLFW/glfw3.h" // Include this header last always to avoid conflicts with loading new OpenGL versions

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"

#include "camera.h"
#include "lightDefs.h"
#include "meshGenerator.h"
#include "noiseMapGenerator.h"
#include "sceneControl.h"
#include "shaderLoader.h"
#include "terrainDefs.h"
#include "textureGenerator.h"
#include "timeMeasureUtils.h"
#include "uniformDefs.h"

#include <iostream>

void renderScene();
void freeResources();

struct WindowData {
  GLFWwindow *window = nullptr;
  int width = 1920;
  int height = 1280;
  glm::dvec2 center = glm::vec2(width / 2.0f, height / 2.0f);
} windowData;

ControlInputData controlInputData;

FrameTimeData frameTimeData = {};

struct ViewFrustumData {
  float fieldOfView = 45.0f;
  float nearPlane = 0.1f;
  float farPlane = 10000.0f;
} viewFrustumData;

Camera fpsCamera(glm::vec3(113.0f, 208.0f, 330.0f), glm::vec3(1.0f, glm::half_pi<float>(), -2.41f));

// UI settings
struct ViewSettings {
  enum class RENDER_MODE { NOISE_MAP, COLOR_MAP, MESH, WIREFRAME };

  RENDER_MODE renderMode = RENDER_MODE::MESH;
  GLuint selectedProgramObject = -1;
  bool showImGuiDemo = false;
} viewSettings;

struct ControlSettings {
  enum class CONTROL_MODE { LIGHT, CAMERA };
  CONTROL_MODE controlMode = CONTROL_MODE::CAMERA;
} controlSettings;

// Light
LightData lightData;

// Terrain
TerrainData terrainData;

Mesh terrainMesh = {};
GLuint terrainGeneratorProgramObject;
GLuint terrainDebugProgramObject;

static void errorCallback(int error, const char *description) { fprintf(stderr, "Error: %s\n", description); }

static void frameBufferSizeCallBack(GLFWwindow *window, int width, int height) {
  // When minimizing
  if (width == 0 || height == 0) {
    return;
  }

  glViewport(0, 0, width, height);
  setUniform(terrainGeneratorProgramObject, ufViewportSizeName, glm::vec2(width, height));
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
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureKeyboard)
    return;

  if (key > 0 && key < 256) {
    if (action == GLFW_PRESS) {
      controlInputData.keyState[key] = true;
    } else if (action == GLFW_RELEASE) {
      controlInputData.keyState[key] = false;
    }
  }

  if (controlInputData.keyState[GLFW_KEY_M]) {
    viewSettings.showImGuiDemo = !viewSettings.showImGuiDemo;
  }

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void initTerrainData() {
  const int mapSize = 256;
  // Only allow power of two
  assert(mapSize && !(mapSize & (mapSize - 1)));

  terrainData.noiseMapData.width = terrainData.noiseMapData.height = mapSize;
  terrainData.noiseMapData.scale = 1.0f;
  terrainData.noiseMapData.octaves = 4;
  terrainData.noiseMapData.persistance = 0.5f;
  terrainData.noiseMapData.lacunarity = 2.0f;
  terrainData.noiseMapData.seed = 1;
  terrainData.noiseMapData.octaveOffset = glm::vec2(0.0f);

  terrainData.noiseMap = generateNoiseMap(terrainData.noiseMapData);

  terrainData.terrainTypes.reserve(3);
  terrainData.terrainTypes.push_back({"Water", glm::vec3(0.0f, 0.0f, 1.0f), 0.4f});
  terrainData.terrainTypes.push_back({"Shallow water", glm::vec3(0.05f, 0.4f, 1.0f), 0.45f});
  terrainData.terrainTypes.push_back({"Sand", glm::vec3(1.0f, 1.0f, 0.45f), 0.495f});
  terrainData.terrainTypes.push_back({"Land", glm::vec3(0.0f, 1.0f, 0.0f), 0.75f});
  terrainData.terrainTypes.push_back({"Mountain", glm::vec3(0.43f, 0.227f, 0.03f), 0.881f});
  terrainData.terrainTypes.push_back({"Snow", glm::vec3(1.0f, 1.0f, 1.0f), 1.0f});
}

void initMeshes(const NoiseMap &noiseMap) {
  // Terrain mesh
  terrainMesh = generateMeshFromHeightMap(noiseMap);

  glGenBuffers(1, &terrainMesh.vboHandle);
  createVertexBufferObject(&terrainMesh.vboHandle, terrainMesh.vertices);

  glGenBuffers(1, &terrainMesh.iboHandle);
  createIndexBufferObject(&terrainMesh.iboHandle, terrainMesh.indices);

  glGenVertexArrays(1, &terrainMesh.vaoHandle);
  glBindVertexArray(terrainMesh.vaoHandle);

  glBindBuffer(GL_ARRAY_BUFFER, terrainMesh.vboHandle);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainMesh.iboHandle);

  glBindVertexArray(0);
}

void initTextures(const NoiseMap &noiseMap) {
  glGenTextures(2, terrainMesh.textureHandles);
  createTexture2D(&terrainMesh.textureHandles[0], GL_CLAMP_TO_EDGE, GL_NEAREST, terrainData.noiseMapData.width,
                  terrainData.noiseMapData.height, generateNoiseMapTexture(noiseMap).data());
  createTexture2D(&terrainMesh.textureHandles[1], GL_CLAMP_TO_EDGE, GL_NEAREST, terrainData.noiseMapData.width,
                  terrainData.noiseMapData.height, generateColorMapTexture(noiseMap, terrainData.terrainTypes).data());
}

void initShaders() {
  const auto viewToClipMatrix =
      glm::perspective(viewFrustumData.fieldOfView, float(windowData.width) / float(windowData.height),
                       viewFrustumData.nearPlane, viewFrustumData.farPlane);

  // Terrain generator shader
  std::vector<GLuint> terrainGeneratorShaderObjects;
  terrainGeneratorShaderObjects.push_back(compileShader("terrain.vert", GL_VERTEX_SHADER));
  terrainGeneratorShaderObjects.push_back(compileShader("terrain.tesc", GL_TESS_CONTROL_SHADER));
  terrainGeneratorShaderObjects.push_back(compileShader("terrain.tese", GL_TESS_EVALUATION_SHADER));
  terrainGeneratorShaderObjects.push_back(compileShader("terrain.frag", GL_FRAGMENT_SHADER));
  terrainGeneratorProgramObject = createProgramObject(terrainGeneratorShaderObjects);

  setUniform(terrainGeneratorProgramObject, ufAmbientConstantName, lightData.ambientConstant);
  setUniform(terrainGeneratorProgramObject, ufHeightMapTextureName, 0);
  setUniform(terrainGeneratorProgramObject, ufColorMapTextureName, 1);
  setUniform(terrainGeneratorProgramObject, ufHeightMultiplierName, terrainData.heightMultiplier);
  setUniform(terrainGeneratorProgramObject, ufPatchSizeName, PATCH_SIZE);
  setUniform(terrainGeneratorProgramObject, ufPixelsPerTriangleName, terrainData.pixelsPerTriangle);
  setUniform(terrainGeneratorProgramObject, ufTerrainGridPointSpacingName, terrainData.gridPointSpacing);
  setUniform(terrainGeneratorProgramObject, ufViewportSizeName, glm::vec2(windowData.width, windowData.height));
  setUniform(terrainGeneratorProgramObject, ufViewToClipMatrixName, viewToClipMatrix);
  setUniform(terrainGeneratorProgramObject, ufWorldLightName, lightData.worldLightPosition);

  // Terrain noise map shader
  std::vector<GLuint> terrainDebugShaderObjects;
  terrainDebugShaderObjects.push_back(compileShader("terrain.vert", GL_VERTEX_SHADER));
  terrainDebugShaderObjects.push_back(compileShader("terrainDebug.tesc", GL_TESS_CONTROL_SHADER));
  terrainDebugShaderObjects.push_back(compileShader("terrainDebug.tese", GL_TESS_EVALUATION_SHADER));
  terrainDebugShaderObjects.push_back(compileShader("terrainDebug.frag", GL_FRAGMENT_SHADER));
  terrainDebugProgramObject = createProgramObject(terrainDebugShaderObjects);

  setUniform(terrainDebugProgramObject, ufUseNoiseMapTextureName, 1);
  setUniform(terrainDebugProgramObject, ufHeightMapTextureName, 0);
  setUniform(terrainDebugProgramObject, ufColorMapTextureName, 1);
  setUniform(terrainDebugProgramObject, ufViewToClipMatrixName, viewToClipMatrix);
}

void initGL() {
  initShaders();

  viewSettings.selectedProgramObject = terrainGeneratorProgramObject;

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST); // Enable depth testing
  glEnable(GL_CULL_FACE);  // Enable backface-culling
  glFrontFace(GL_CCW);
  glViewport(0, 0, windowData.width, windowData.height);

  // Number of input control points for patch in Tess. Control Shader
  glPatchParameteri(GL_PATCH_VERTICES, 1);
}

void updateMapTexture() {
  terrainData.noiseMap = generateNoiseMap(terrainData.noiseMapData);
  updateTexture2D(&terrainMesh.textureHandles[0], 0, 0, terrainData.noiseMapData.width, terrainData.noiseMapData.height,
                  generateNoiseMapTexture(terrainData.noiseMap).data());
  updateTexture2D(&terrainMesh.textureHandles[1], 0, 0, terrainData.noiseMapData.width, terrainData.noiseMapData.height,
                  generateColorMapTexture(terrainData.noiseMap, terrainData.terrainTypes).data());
}

void renderImGuiInterface() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if (viewSettings.showImGuiDemo)
    ImGui::ShowDemoWindow(&viewSettings.showImGuiDemo);

  ImGui::Begin("Settings", nullptr,
               ImGuiWindowFlags_AlwaysAutoResize); // Create a window called "Noise map settings"
                                                   // and append into it.

  // View settings
  ImGui::Text("Render mode");
  if (ImGui::Button("Noise Map")) {
    viewSettings.renderMode = ViewSettings::RENDER_MODE::NOISE_MAP;
    viewSettings.selectedProgramObject = terrainDebugProgramObject;
  }
  ImGui::SameLine();
  if (ImGui::Button("Color Map")) {
    viewSettings.renderMode = ViewSettings::RENDER_MODE::COLOR_MAP;
    viewSettings.selectedProgramObject = terrainDebugProgramObject;
  }
  ImGui::SameLine();
  if (ImGui::Button("Mesh")) {
    viewSettings.renderMode = ViewSettings::RENDER_MODE::MESH;
    viewSettings.selectedProgramObject = terrainGeneratorProgramObject;
  }
  ImGui::SameLine();
  if (ImGui::Button("Wireframe Mesh")) {
    viewSettings.renderMode = ViewSettings::RENDER_MODE::WIREFRAME;
    viewSettings.selectedProgramObject = terrainGeneratorProgramObject;
  }

  ImGui::Text("Control mode");
  if (ImGui::Button("Camera")) {
    controlSettings.controlMode = ControlSettings::CONTROL_MODE::CAMERA;
  }
  ImGui::SameLine();
  if (ImGui::Button("Light")) {
    controlSettings.controlMode = ControlSettings::CONTROL_MODE::LIGHT;
  }

  ImGui::Text("Terrain settings");
  if (ImGui::SliderInt("Pixels per triangle", &terrainData.pixelsPerTriangle, 1, 30)) {
    setUniform(terrainGeneratorProgramObject, ufPixelsPerTriangleName, terrainData.pixelsPerTriangle);
  }

  ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
  if (ImGui::TreeNode("Noise Map settings")) {
    if (ImGui::SliderFloat("Scale", &terrainData.noiseMapData.scale, 1.0f, 10.0f) ||
        ImGui::SliderInt("Octaves", &terrainData.noiseMapData.octaves, 1, 6) ||
        ImGui::SliderFloat("Persistance", &terrainData.noiseMapData.persistance, 0.0f, 1.0f) ||
        ImGui::SliderFloat("Lacunarity", &terrainData.noiseMapData.lacunarity, 2.0f, 4.0f) ||
        ImGui::SliderInt("Seed", &terrainData.noiseMapData.seed, 1, 100) ||
        ImGui::SliderFloat("Octave offset X", &terrainData.noiseMapData.octaveOffset.x, 0.0f, 2000.0f) ||
        ImGui::SliderFloat("Octave offset Y", &terrainData.noiseMapData.octaveOffset.y, 0.0f, 2000.0f)) {
      updateMapTexture();
    }

    ImGui::TreePop();
  }

  ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
  if (ImGui::TreeNode("Terrain type settings")) {
    if (ImGui::SliderFloat("Terrain grid spacing", &terrainData.gridPointSpacing, 1.0f, 10.0f)) {
      setUniform(terrainGeneratorProgramObject, ufTerrainGridPointSpacingName, terrainData.gridPointSpacing);
    }
    if (ImGui::SliderFloat("Height multiplier", &terrainData.heightMultiplier, 0.0f, 100.0f)) {
      setUniform(terrainGeneratorProgramObject, ufHeightMultiplierName, terrainData.heightMultiplier);
    }

    for (size_t i = 0; i < terrainData.terrainTypes.size(); i++) {
      ImGui::PushID(int(i));
      if (ImGui::TreeNode(terrainData.terrainTypes[i].name.data())) {
        if (ImGui::SliderFloat("Height", &terrainData.terrainTypes[i].height, 0.0f, 1.0f) ||
            ImGui::ColorEdit3("Color", glm::value_ptr(terrainData.terrainTypes[i].color),
                              ImGuiColorEditFlags_NoInputs)) {
          updateMapTexture();
        }
        ImGui::TreePop();
      }
      ImGui::PopID();
    }

    ImGui::TreePop();
  }

  ImGui::End();

  ImGui::Render();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
/*
const float frustumTolerance = 0.0001f;
bool patchEdgeInFrustum(const glm::vec4 p1, const glm::vec4 p2) {
  auto minusP1W = -p1.w;
  auto minusP2W = -p2.w;

  if ((p1.x >= (-p1.w - frustumTolerance) || p2.x >= (-p2.w - frustumTolerance)) &&
      (p1.x <= (p1.w + frustumTolerance) || p2.x <= (p2.w + frustumTolerance)) &&
      (p1.z >= (-p1.w - frustumTolerance) || p2.z >= (-p2.w - frustumTolerance)) &&
      (p1.z <= (p1.w + frustumTolerance) || p2.z <= (p2.w + frustumTolerance))) {
    return true;
  }
  return false;
}

bool patchTest(const glm::vec4 p1) {
  if (p1.x >= -p1.w && p1.x <= p1.w && p1.z >= -p1.w && p1.z <= p1.w) {
    return true;
  }
  return false;
}
*/
void renderScene() {
  glfwPollEvents();
  if (controlSettings.controlMode == ControlSettings::CONTROL_MODE::CAMERA) {
    handleCameraInput(&fpsCamera, controlInputData, frameTimeData.frameTime);
  } else if (controlSettings.controlMode == ControlSettings::CONTROL_MODE::LIGHT) {
    handleLightInput(&lightData, controlInputData, frameTimeData.frameTime);
  }

  /*glm::vec2 tSize(terrainData.noiseMapData.width, terrainData.noiseMapData.height);
  float div = PATCH_SIZE / tSize.x;

  for (uint32_t i = 0; i < terrainMesh.vertices.size(); i++) {
    const auto patchLowerLeftCorner = terrainMesh.vertices[i].position2f;

    glm::vec2 patchCornersTexCoord[4];
    patchCornersTexCoord[0] = patchLowerLeftCorner;
    patchCornersTexCoord[1] = glm::vec2(patchLowerLeftCorner.x, patchLowerLeftCorner.y + div);
    patchCornersTexCoord[2] = glm::vec2(patchLowerLeftCorner.x + div, patchLowerLeftCorner.y);
    patchCornersTexCoord[3] = glm::vec2(patchLowerLeftCorner.x + div, patchLowerLeftCorner.y + div);

    glm::vec2 patchCorners[4];
    for (int i = 0; i < 4; ++i) {
      patchCorners[i] = patchCornersTexCoord[i] * tSize * terrainData.gridPointSpacing;
    }

    const auto viewMatrixTest = fpsCamera.createViewMatrix();

    const auto viewToClipMatrixTest =
        glm::perspective(viewFrustumData.fieldOfView, float(windowData.width) / float(windowData.height),
                         viewFrustumData.nearPlane, viewFrustumData.farPlane);

    const glm::mat4 mvp = viewToClipMatrixTest * viewMatrixTest * terrainMesh.modelTransformation;

    glm::vec4 clipSpacePatchCorners[4];
    clipSpacePatchCorners[0] =
        mvp * glm::vec4(patchCorners[0].x, terrainData.heightMultiplier, patchCorners[0].y, 1.0f);
    clipSpacePatchCorners[1] =
        mvp * glm::vec4(patchCorners[1].x, terrainData.heightMultiplier, patchCorners[1].y, 1.0f);
    clipSpacePatchCorners[2] =
        mvp * glm::vec4(patchCorners[2].x, terrainData.heightMultiplier, patchCorners[2].y, 1.0f);
    clipSpacePatchCorners[3] =
        mvp * glm::vec4(patchCorners[3].x, terrainData.heightMultiplier, patchCorners[3].y, 1.0f);

   //if (patchEdgeInFrustum(clipSpacePatchCorners[0], clipSpacePatchCorners[0 + 1]) ||
   //    patchEdgeInFrustum(clipSpacePatchCorners[0], clipSpacePatchCorners[0 + 2]) ||
   //    patchEdgeInFrustum(clipSpacePatchCorners[0 + 2], clipSpacePatchCorners[0 + 3]) ||
   //    patchEdgeInFrustum(clipSpacePatchCorners[0 + 3], clipSpacePatchCorners[0 + 1])) {
   //  std::cout << "Inside patch: " << i << std::endl;
   //} else {
   //  std::cout << "Patch culled: " << i << std::endl;
   //}
    if (patchTest(clipSpacePatchCorners[0]) || patchTest(clipSpacePatchCorners[1]) ||
        patchTest(clipSpacePatchCorners[2]) || patchTest(clipSpacePatchCorners[3])) {
      std::cout << "Inside patch: " << i << std::endl;
    } else {
      std::cout << "Patch culled: " << i << std::endl;
    }
  }*/

  // Measure time each frame
  updateFrameTime(&frameTimeData);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (viewSettings.renderMode == ViewSettings::RENDER_MODE::NOISE_MAP) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[0]);
    setUniform(viewSettings.selectedProgramObject, ufUseNoiseMapTextureName, 1);
  } else if (viewSettings.renderMode == ViewSettings::RENDER_MODE::COLOR_MAP) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[1]);
    setUniform(viewSettings.selectedProgramObject, ufUseNoiseMapTextureName, 0);
  } else if (viewSettings.renderMode == ViewSettings::RENDER_MODE::MESH ||
             viewSettings.renderMode == ViewSettings::RENDER_MODE::WIREFRAME) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[0]); // Color map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[1]); // Height map
  }

  glBindVertexArray(terrainMesh.vaoHandle);
  glUseProgram(viewSettings.selectedProgramObject);
  {
    setUniform(viewSettings.selectedProgramObject, ufModelToWorldMatrixName, terrainMesh.modelTransformation);

    const auto viewMatrix = fpsCamera.createViewMatrix();
    setUniform(viewSettings.selectedProgramObject, ufWorldToViewMatrixName, viewMatrix);
    setUniform(viewSettings.selectedProgramObject, ufNormalMatrix,
               glm::transpose(glm::inverse(glm::mat3(viewMatrix * terrainMesh.modelTransformation))));

    const auto viewToClipMatrix =
        glm::perspective(viewFrustumData.fieldOfView, float(windowData.width) / float(windowData.height),
                         viewFrustumData.nearPlane, viewFrustumData.farPlane);
    setUniform(viewSettings.selectedProgramObject, ufViewToClipMatrixName, viewToClipMatrix);

    setUniform(terrainGeneratorProgramObject, ufWorldLightName, lightData.worldLightPosition);

    if (viewSettings.renderMode == ViewSettings::RENDER_MODE::WIREFRAME) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDrawElements(GL_PATCHES, GLsizei(terrainMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
  }
  glUseProgram(0);

  renderImGuiInterface();
  glfwSwapBuffers(windowData.window);
}

void initImGui(GLFWwindow *window, const std::string &glslVersion) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glslVersion.c_str());

  ImGui::StyleColorsDark();
}

void initScene() {
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

  initImGui(windowData.window, "#version 130");

  initGL();

  initTerrainData();
  initMeshes(terrainData.noiseMap);
  initTextures(terrainData.noiseMap);

  while (!glfwWindowShouldClose(windowData.window)) {
    renderScene();
  }

  freeResources();
}

void freeResources() {
  glDeleteBuffers(1, &terrainMesh.vboHandle);
  glDeleteBuffers(1, &terrainMesh.iboHandle);
  for (auto &textureHandle : terrainMesh.textureHandles) {
    glDeleteTextures(1, &textureHandle);
  }

  deleteProgramObject(terrainGeneratorProgramObject);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(windowData.window);
  glfwTerminate();
}

int main(int argc, char **argv) {
  initScene();
  return 0;
}