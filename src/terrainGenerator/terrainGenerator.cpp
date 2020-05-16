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
#include "shaderLoader.h"
#include "meshGenerator.h"
#include "noiseMapGenerator.h"
#include "terrainDefs.h"
#include "textureGenerator.h"
#include "timeMeasureUtils.h"

#include <iostream>

void renderScene();
void freeResources();

struct WindowData {
  GLFWwindow* window = nullptr;
  int width = 1920;
  int height = 1280;
  glm::dvec2 center = glm::vec2(width / 2.0f, height / 2.0f);
} windowData;

glm::dvec2 previousMousePosition;

bool keyState[256] = { false };

FrameTimeData frameTimeData = {};

struct ProjectionData {
  float fieldOfView = 45.0f;
  float nearPlane = 0.1f;
  float farPlane = 1000.0f;
} projectionData;


Camera fpsCamera(glm::vec3(58.0f, 132.0f, 76.0f), glm::vec3(1.0f, glm::half_pi<float>(), -2.41f));
Camera quadMeshCamera; // debugging

GLuint terrainGeneratorProgramObject;
GLuint debugMeshProgramObject;

constexpr auto ufUseMapTextureName = "useMapTexture";
constexpr auto ufHeightMapTextureName = "heightMapTexture";
constexpr auto ufColorMapTextureName = "colorMapTexture";
constexpr auto ufModelToWorldMatrixName = "modelToWorldMatrix";
constexpr auto ufWorldToViewMatrixName = "worldToViewMatrix";
constexpr auto ufViewToClipMatrixName = "viewToClipMatrix";
constexpr auto ufNormalMatrix = "normalMatrix";
constexpr auto ufWorldLightName = "worldLight";
constexpr auto ufAmbientConstantName = "ambientConstant";
constexpr auto ufterrainGridPointSpacingName = "terrainGridPointSpacing";
constexpr auto ufHeightMultiplierName = "heightMultiplier";

// Lightning data
const glm::vec4 worldLightPosition(64.0f, 145.0f, 30.0f, 1.0f);
const glm::vec3 ambientConstant(0.3f, 0.3f, 0.3f);

// Terrain data
NoiseMapData noiseMapData = {};
std::vector<TerrainType> terrainTypes;
float terrainGridPointSpacing = 1.0f;
float heightMultiplier = 1.0f;
Mesh quadMesh = {};
Mesh terrainMesh = {};

enum class RENDER_MODE { NOISE_MAP, COLOR_MAP, MESH };
struct {
  std::array<char, TERRAIN_NAME_SIZE> newTerrainTypeName = {};
  RENDER_MODE renderMode = RENDER_MODE::MESH;
  Camera* selectedCamera = nullptr;
  GLuint selectedProgramObject = -1;
  bool showImGuiDemo = false;
  bool terrainSettingCloseButton = false;
  bool wireframe = false;
} terrainSettings;

static void errorCallback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void resizeWindowCallback(GLFWwindow* window, int width, int height) {
  const auto viewToClipMatrix =
    glm::perspective(projectionData.fieldOfView, float(width) / float(height),
      projectionData.nearPlane, projectionData.farPlane);
  setUniform(terrainSettings.selectedProgramObject, ufViewToClipMatrixName, viewToClipMatrix);
  glViewport(0, 0, width, height);
}

static void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
  glm::dvec2 mousePosition(xPos, yPos);
  glm::dvec2 mouseDiff = mousePosition - previousMousePosition;
  previousMousePosition = mousePosition;

  GLfloat mouseSpeed = 0.001f;
  terrainSettings.selectedCamera->yawRotation(GLfloat(mouseDiff.x * mouseSpeed));
  terrainSettings.selectedCamera->pitchRotation(GLfloat(mouseDiff.y * mouseSpeed));
}

static void handleKeyboardInput() {
  GLfloat cameraSpeed = 10.0f * float(frameTimeData.frameTime);
  if (terrainSettings.renderMode == RENDER_MODE::MESH) {
    cameraSpeed *= 5.0f;
  }

  GLfloat radian = 0.017453293f; // One radian

  if (keyState[GLFW_KEY_W])
    terrainSettings.selectedCamera->moveForward(cameraSpeed);
  if (keyState[GLFW_KEY_S])
    terrainSettings.selectedCamera->moveBackward(cameraSpeed);
  if (keyState[GLFW_KEY_A])
    terrainSettings.selectedCamera->moveLeft(cameraSpeed);
  if (keyState[GLFW_KEY_D])
    terrainSettings.selectedCamera->moveRight(cameraSpeed);
  if (keyState[GLFW_KEY_Q])
    terrainSettings.selectedCamera->moveUp(cameraSpeed);
  if (keyState[GLFW_KEY_E])
    terrainSettings.selectedCamera->moveDown(cameraSpeed);
  if (keyState[GLFW_KEY_J])
    terrainSettings.selectedCamera->yawRotation(-radian);
  if (keyState[GLFW_KEY_L])
    terrainSettings.selectedCamera->yawRotation(radian);
  if (keyState[GLFW_KEY_I])
    terrainSettings.selectedCamera->pitchRotation(radian);
  if (keyState[GLFW_KEY_K])
    terrainSettings.selectedCamera->pitchRotation(-radian);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  ImGuiIO& io = ImGui::GetIO();
  if (io.WantCaptureKeyboard)
    return;

  if (key > 0 && key < 256) {
    if (action == GLFW_PRESS) {
      keyState[key] = true;
    }
    else if (action == GLFW_RELEASE) {
      keyState[key] = false;
    }
  }

  if (keyState[GLFW_KEY_M]) {
    terrainSettings.showImGuiDemo = !terrainSettings.showImGuiDemo;
  }

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void initTerrainTypeList() {
  TerrainType waterTerrain = {};
  strcpy_s(waterTerrain.name.data(), waterTerrain.name.size(), "Water");
  waterTerrain.color = glm::vec3(0.0f, 0.0f, 1.0f);
  waterTerrain.height = 0.3f;

  TerrainType landTerrain = {};
  strcpy_s(landTerrain.name.data(), landTerrain.name.size(), "Land");
  landTerrain.color = glm::vec3(0.0f, 1.0f, 0.0f);
  landTerrain.height = 1.0f;

  terrainTypes.reserve(3);
  terrainTypes.push_back(waterTerrain);
  terrainTypes.push_back(landTerrain);
}

void initMeshes() {
  noiseMapData.width = noiseMapData.height = 512;
  noiseMapData.scale = 27.6f;
  noiseMapData.octaves = 4;
  noiseMapData.persistance = 0.5f;
  noiseMapData.lacunarity = 2.0f;
  noiseMapData.seed = 1;
  noiseMapData.octaveOffset = glm::vec2(0.0f);

  const auto noiseMap = generateNoiseMap(noiseMapData);
  const auto colorMapTexture = generateColorMapTexture(noiseMap, terrainTypes);

  // Quad mesh for debugging
  quadMesh = generateQuadMesh();

  glGenBuffers(1, &quadMesh.vboHandle);
  glBindBuffer(GL_ARRAY_BUFFER, quadMesh.vboHandle);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * quadMesh.vertices.size(), quadMesh.vertices.data(),
    GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &quadMesh.iboHandle);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadMesh.iboHandle);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * quadMesh.indices.size(),
    quadMesh.indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glGenTextures(1, &quadMesh.textureHandles[0]);
  glBindTexture(GL_TEXTURE_2D, quadMesh.textureHandles[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, noiseMapData.width, noiseMapData.height, 0, GL_RGB,
    GL_FLOAT, colorMapTexture.data());
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenVertexArrays(1, &quadMesh.vaoHandle);
  glBindVertexArray(quadMesh.vaoHandle);

  glBindBuffer(GL_ARRAY_BUFFER, quadMesh.vboHandle);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(4 * sizeof(float)));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadMesh.iboHandle);
  glBindVertexArray(0);

  // Terrain mesh

  terrainMesh = generateMeshFromHeightMap(noiseMap);

  glGenBuffers(1, &terrainMesh.vboHandle);
  glBindBuffer(GL_ARRAY_BUFFER, terrainMesh.vboHandle);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * terrainMesh.vertices.size(),
    terrainMesh.vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &terrainMesh.iboHandle);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainMesh.iboHandle);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * terrainMesh.indices.size(),
    terrainMesh.indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Color map texture (TODO: Refactor, make a method that sets texture!!!!!!!!!)
  glGenTextures(2, terrainMesh.textureHandles);

  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, noiseMapData.width, noiseMapData.height, 0, GL_RGB,
    GL_FLOAT, generateNoiseMapTexture(noiseMap).data());
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, noiseMapData.width, noiseMapData.height, 0, GL_RGB,
    GL_FLOAT, colorMapTexture.data());
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenVertexArrays(1, &terrainMesh.vaoHandle);
  glBindVertexArray(terrainMesh.vaoHandle);

  glBindBuffer(GL_ARRAY_BUFFER, terrainMesh.vboHandle);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainMesh.iboHandle);

  glBindVertexArray(0);
}

void initShaders() {
  const auto viewToClipMatrix = glm::perspective(projectionData.fieldOfView,
    float(windowData.width) / float(windowData.height),
    projectionData.nearPlane, projectionData.farPlane);
  // Quad mesh shader
  std::vector<GLuint> quadMeshShaderObjects;
  quadMeshShaderObjects.push_back(compileShader("debugMesh.vert", GL_VERTEX_SHADER));
  quadMeshShaderObjects.push_back(compileShader("debugMesh.frag", GL_FRAGMENT_SHADER));
  debugMeshProgramObject = createProgramObject(quadMeshShaderObjects);

  setUniform(debugMeshProgramObject, ufViewToClipMatrixName, viewToClipMatrix);

  // Terrain generator shader
  std::vector<GLuint> terrainGeneratorShaderObjects;
  terrainGeneratorShaderObjects.push_back(compileShader("terrainTest.vert", GL_VERTEX_SHADER));
  terrainGeneratorShaderObjects.push_back(
    compileShader("terrainTest.tesc", GL_TESS_CONTROL_SHADER));
  terrainGeneratorShaderObjects.push_back(
    compileShader("terrainTest.tese", GL_TESS_EVALUATION_SHADER));
  terrainGeneratorShaderObjects.push_back(compileShader("terrainTest.frag", GL_FRAGMENT_SHADER));
  terrainGeneratorProgramObject = createProgramObject(terrainGeneratorShaderObjects);

  setUniform(terrainGeneratorProgramObject, ufUseMapTextureName, 1);
  setUniform(terrainGeneratorProgramObject, ufHeightMapTextureName, 0);
  setUniform(terrainGeneratorProgramObject, ufColorMapTextureName, 1);
  setUniform(terrainGeneratorProgramObject, ufViewToClipMatrixName, viewToClipMatrix);
  setUniform(terrainGeneratorProgramObject, ufWorldLightName, worldLightPosition);
  setUniform(terrainGeneratorProgramObject, ufAmbientConstantName, ambientConstant);
  setUniform(terrainGeneratorProgramObject, ufterrainGridPointSpacingName, terrainGridPointSpacing);
  setUniform(terrainGeneratorProgramObject, ufHeightMultiplierName, heightMultiplier);
}

void initGL() {
  initShaders();

  terrainSettings.selectedProgramObject = terrainGeneratorProgramObject;
  terrainSettings.selectedCamera = &fpsCamera;

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST); // Enable depth testing
  glEnable(GL_CULL_FACE);  // Enable backface-culling
  glFrontFace(GL_CCW);
  glViewport(0, 0, windowData.width, windowData.height);

  // Number of input control points for patch in Tess. Control Shader
  glPatchParameteri(GL_PATCH_VERTICES, 1);

  initMeshes();
}

void updateMapTexture() {
  noiseMapData.width = glm::clamp(noiseMapData.width, 1, noiseMapData.width);
  noiseMapData.height = noiseMapData.width;

  noiseMapData.scale = glm::clamp(noiseMapData.scale, 0.0001f, noiseMapData.scale);
  noiseMapData.octaves = glm::clamp(noiseMapData.octaves, 1, 6);
  noiseMapData.lacunarity = glm::clamp(noiseMapData.lacunarity, 2.0f, 4.0f);
  noiseMapData.persistance = glm::clamp(noiseMapData.persistance, 0.0f, 1.0f);
  noiseMapData.seed = glm::clamp(noiseMapData.seed, 1, 100);
  const auto noiseMap = generateNoiseMap(noiseMapData);

  /*const auto noiseMap = generateNoiseMap(noiseMapData);
  std::vector<glm::vec3> mapTexture;
  if (terrainSettings.renderMode == RENDER_MODE::NOISE_MAP) {
    mapTexture = generateNoiseMapTexture(noiseMap);
    glBindTexture(GL_TEXTURE_2D, quadMesh.textureHandles[0]);
  }
  else if (terrainSettings.renderMode == RENDER_MODE::COLOR_MAP) {
    mapTexture = generateColorMapTexture(noiseMap, terrainTypes);
    glBindTexture(GL_TEXTURE_2D, quadMesh.textureHandles[0]);
  }
  else if (terrainSettings.renderMode == RENDER_MODE::MESH) {
    mapTexture = generateNoiseMapTexture(noiseMap);
    glBindTexture(GL_TEXTURE_2D, quadMesh.textureHandles[0]);
    mapTexture = generateColorMapTexture(noiseMap, terrainTypes);
    glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[1]);
  }*/

  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[0]);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, noiseMapData.width, noiseMapData.height, GL_RGB, GL_FLOAT,
    generateNoiseMapTexture(noiseMap).data());
  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[1]);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, noiseMapData.width, noiseMapData.height, GL_RGB, GL_FLOAT,
    generateColorMapTexture(noiseMap, terrainTypes).data());
}

void renderImGuiInterface() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if (terrainSettings.showImGuiDemo)
    ImGui::ShowDemoWindow(&terrainSettings.showImGuiDemo);

  ImGui::Begin("Terrain settings", &terrainSettings.terrainSettingCloseButton,
    ImGuiWindowFlags_AlwaysAutoResize); // Create a window called "Noise map settings"
                                        // and append into it.
  ImGui::Text("Render mode");
  if (ImGui::Button("Noise Map")) {
    terrainSettings.renderMode = RENDER_MODE::NOISE_MAP;
    terrainSettings.selectedProgramObject = debugMeshProgramObject;
    terrainSettings.selectedCamera = &quadMeshCamera;
    updateMapTexture();
  }
  ImGui::SameLine();
  if (ImGui::Button("Color Map")) {
    terrainSettings.renderMode = RENDER_MODE::COLOR_MAP;
    terrainSettings.selectedProgramObject = debugMeshProgramObject;
    terrainSettings.selectedCamera = &quadMeshCamera;
    updateMapTexture();
  }
  ImGui::SameLine();
  if (ImGui::Button("Mesh")) {
    terrainSettings.wireframe = false;
    terrainSettings.renderMode = RENDER_MODE::MESH;
    terrainSettings.selectedProgramObject = terrainGeneratorProgramObject;
    terrainSettings.selectedCamera = &fpsCamera;
    updateMapTexture();
  }
  ImGui::SameLine();
  if (ImGui::Button("Wireframe Mesh")) {
    terrainSettings.wireframe = true;
    terrainSettings.renderMode = RENDER_MODE::MESH;
    terrainSettings.selectedProgramObject = terrainGeneratorProgramObject;
    terrainSettings.selectedCamera = &fpsCamera;
    updateMapTexture();
  }

  ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
  if (ImGui::TreeNode("Noise Map settings")) {
    if (ImGui::SliderFloat("Scale", &noiseMapData.scale, 0.1f, 100.0f) ||
      ImGui::SliderInt("Octaves", &noiseMapData.octaves, 1, 6) ||
      ImGui::SliderFloat("Persistance", &noiseMapData.persistance, 0.0f, 1.0f) ||
      ImGui::SliderFloat("Lacunarity", &noiseMapData.lacunarity, 2.0f, 4.0f) ||
      ImGui::SliderInt("Seed", &noiseMapData.seed, 1, 100) ||
      ImGui::SliderFloat("Octave offset X", &noiseMapData.octaveOffset.x, 0.0f, 10.0f) ||
      ImGui::SliderFloat("Octave offset Y", &noiseMapData.octaveOffset.y, 0.0f, 10.0f)) {
      updateMapTexture();
    }

    ImGui::TreePop();
  }

  ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
  if (ImGui::TreeNode("Terrain settings")) {
    if (ImGui::SliderFloat("Terrain grid spacing", &terrainGridPointSpacing, 0.0f, 10.0f)) {
      setUniform(terrainGeneratorProgramObject, ufterrainGridPointSpacingName, terrainGridPointSpacing);
    }
    if (ImGui::SliderFloat("Height multiplier", &heightMultiplier, 0.0f, 500.0f)) {
      setUniform(terrainGeneratorProgramObject, ufHeightMultiplierName, heightMultiplier);
    }

    for (size_t i = 0; i < terrainTypes.size(); i++) {
      ImGui::PushID(int(i));
      if (ImGui::TreeNode(terrainTypes[i].name.data())) {
        if (ImGui::SliderFloat("Height", &terrainTypes[i].height, 0.0f, 1.0f) ||
          ImGui::ColorEdit3("Color", glm::value_ptr(terrainTypes[i].color),
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

void renderScene() {
  glfwPollEvents();
  handleKeyboardInput();

  // Measure time each frame
  updateFrameTime(&frameTimeData);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const Mesh* mesh = nullptr; // should use optional...
  if (terrainSettings.renderMode == RENDER_MODE::NOISE_MAP ||
    terrainSettings.renderMode == RENDER_MODE::COLOR_MAP) {
    mesh = &quadMesh;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh->textureHandles[0]);
  }
  else if (terrainSettings.renderMode == RENDER_MODE::MESH) {
    mesh = &terrainMesh;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh->textureHandles[0]); // Color map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mesh->textureHandles[1]); // Height map
  }
  assert(mesh != nullptr);

  glBindVertexArray(mesh->vaoHandle);
  glUseProgram(terrainSettings.selectedProgramObject);
  {
    setUniform(terrainSettings.selectedProgramObject, ufModelToWorldMatrixName,
      mesh->modelTransformation);

    const auto viewMatrix = terrainSettings.selectedCamera->createViewMatrix();
    setUniform(terrainSettings.selectedProgramObject, ufWorldToViewMatrixName, viewMatrix);
    setUniform(terrainSettings.selectedProgramObject, ufNormalMatrix,
      glm::transpose(glm::inverse(glm::mat3(viewMatrix * mesh->modelTransformation))));

    if (terrainSettings.wireframe) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDrawElements(GL_PATCHES, GLsizei(mesh->indices.size()), GL_UNSIGNED_INT, (void*)0);
  }
  glUseProgram(0);

  renderImGuiInterface();
  glfwSwapBuffers(windowData.window);
}

void initImGui(GLFWwindow* window, const std::string& glslVersion) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
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
  windowData.window =
    glfwCreateWindow(windowData.width, windowData.height, "Terrain Generator", NULL, NULL);
  if (!windowData.window) {
    glfwTerminate();
    assert(false);
  }

  glfwSetWindowSizeCallback(windowData.window, resizeWindowCallback);

  glfwSetKeyCallback(windowData.window, keyCallback);

  // Disable mouse movements for now...
  // glfwSetCursorPosCallback(windowData.window, cursorPosCallback); 
  glfwSetCursorPos(windowData.window, windowData.center.x, windowData.center.y);
  previousMousePosition = windowData.center;

  glfwMakeContextCurrent(windowData.window);

  GLenum glewRes = glewInit();
  if (glewRes != GLEW_OK) {
    fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(glewRes));
    glfwTerminate();
    assert(false);
  }

  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  // glfwSwapInterval(1); // Gives input latency

  initTerrainTypeList();

  initImGui(windowData.window, "#version 130");

  initGL();

  while (!glfwWindowShouldClose(windowData.window)) {
    renderScene();
  }

  freeResources();
}

void freeResources() {
  glDeleteBuffers(1, &quadMesh.vboHandle);
  glDeleteBuffers(1, &quadMesh.iboHandle);
  for (auto& textureHandle : quadMesh.textureHandles) {
    glDeleteTextures(1, &textureHandle);
  }

  glDeleteBuffers(1, &terrainMesh.vboHandle);
  glDeleteBuffers(1, &terrainMesh.iboHandle);
  for (auto& textureHandle : terrainMesh.textureHandles) {
    glDeleteTextures(1, &textureHandle);
  }

  deleteProgramObject(debugMeshProgramObject);
  deleteProgramObject(terrainGeneratorProgramObject);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(windowData.window);
  glfwTerminate();
}

int main(int argc, char** argv) {
  initScene();
  return 0;
}