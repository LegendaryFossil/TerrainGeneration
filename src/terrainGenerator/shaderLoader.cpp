#include "ShaderLoader.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>

namespace {

const std::string shaderPath(getExePath() + "/resources/shaders/");

void validateShaderCompileStatus(const std::string &shaderFileName, const GLuint shaderObject,
                                 const GLuint shaderType) {
  GLint status;
  glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint infoLogLength;
    glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar *logInfo = new char[infoLogLength + 1];
    glGetShaderInfoLog(shaderObject, infoLogLength, NULL, logInfo);

    const char *strShaderType = NULL;
    switch (shaderType) {
    case GL_VERTEX_SHADER:
      strShaderType = "vertex";
      break;

    case GL_TESS_CONTROL_SHADER:
      strShaderType = "tessellation control";
      break;

    case GL_TESS_EVALUATION_SHADER:
      strShaderType = "tessellation evaluation";
      break;

    case GL_GEOMETRY_SHADER:
      strShaderType = "geometry";
      break;

    case GL_FRAGMENT_SHADER:
      strShaderType = "fragment";
      break;
    }

    fprintf(stderr, "Compile failure in %s (%s) shader:\n%s\n", shaderFileName.c_str(), strShaderType,
            logInfo);
    delete[] logInfo;
    assert(false);
  }
}

void validateProgramObjectLinkStatus(const GLuint programObject) {
  GLint status;
  glGetProgramiv(programObject, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint infoLogLength;
    glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar *logInfo = new char[infoLogLength + 1];
    glGetProgramInfoLog(programObject, infoLogLength, NULL, logInfo);
    fprintf(stderr, "Linker failure: %s\n", logInfo);
    delete[] logInfo;
    assert(false);
  }
}
} // namespace

GLuint compileShader(const std::string &shaderFileName, const GLuint shaderType) {
  GLuint shaderObject = glCreateShader(shaderType);
  std::ifstream shaderFileStream(shaderPath + shaderFileName);
  if (shaderFileStream.is_open()) {
    std::string content((std::istreambuf_iterator<char>(shaderFileStream.rdbuf())),
                        (std::istreambuf_iterator<char>()));
    const char *shaderContent = content.c_str();
    glShaderSource(shaderObject, 1, &shaderContent, NULL);
    glCompileShader(shaderObject);
    validateShaderCompileStatus(shaderFileName, shaderObject, shaderType);
    return shaderObject;
  } else {
    std::cout << "Unable to open shader file: " << shaderFileName << std::endl;
    exit(1);
  }
}

GLuint createProgramObject(const std::vector<GLuint> &shaderObjects) {
  const auto programObject = glCreateProgram();

  for (const auto shaderObject : shaderObjects) {
    glAttachShader(programObject, shaderObject);
  }

  glLinkProgram(programObject);
  validateProgramObjectLinkStatus(programObject);

  for (const auto shaderObject : shaderObjects) {
    glDetachShader(programObject, shaderObject);
  }

  return programObject;
}

void validateProgramObject(const GLuint programObject) {
  glValidateProgram(programObject);
  GLint status;
  glGetProgramiv(programObject, GL_VALIDATE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint infoLogLength;
    glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar *logInfo = new char[infoLogLength + 1];
    glGetProgramInfoLog(programObject, infoLogLength, NULL, logInfo);

    fprintf(stderr, "Program object creation error: %s\n", logInfo);
    delete[] logInfo;
    assert(false);
  }
}

void deleteProgramObject(const GLuint programObject) { glDeleteProgram(programObject); }

// Int
void setUniform(const GLuint programObject, const std::string &uniformName, const int uniformValue) {
  glProgramUniform1iv(programObject, glGetUniformLocation(programObject, uniformName.c_str()), 1,
                      &uniformValue);
}
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::ivec2 &uniformValue) {
  glProgramUniform2iv(programObject, glGetUniformLocation(programObject, uniformName.c_str()), 1,
                      glm::value_ptr(uniformValue));
}
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::ivec3 &uniformValue) {
  glProgramUniform3iv(programObject, glGetUniformLocation(programObject, uniformName.c_str()), 1,
                      glm::value_ptr(uniformValue));
}
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::ivec4 &uniformValue) {
  glProgramUniform4iv(programObject, glGetUniformLocation(programObject, uniformName.c_str()), 1,
                      glm::value_ptr(uniformValue));
}

// Float
void setUniform(const GLuint programObject, const std::string &uniformName, const float uniformValue) {
  glProgramUniform1fv(programObject, glGetUniformLocation(programObject, uniformName.c_str()), 1,
                      &uniformValue);
}
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::vec2 &uniformValue) {
  glProgramUniform2fv(programObject, glGetUniformLocation(programObject, uniformName.c_str()), 1,
                      glm::value_ptr(uniformValue));
}
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::vec3 &uniformValue) {
  glProgramUniform3fv(programObject, glGetUniformLocation(programObject, uniformName.c_str()), 1,
                      glm::value_ptr(uniformValue));
}
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::vec4 &uniformValue) {
  glProgramUniform4fv(programObject, glGetUniformLocation(programObject, uniformName.c_str()), 1,
                      glm::value_ptr(uniformValue));
}

// Float arrays
void setUniform(const GLuint programObject, const std::string &uniformName,
                const std::vector<float> &uniformValues) {
  glProgramUniform1fv(programObject, glGetUniformLocation(programObject, uniformName.c_str()),
                      uniformValues.size(), uniformValues.data());
}

void setUniform(const GLuint programObject, const std::string &uniformName,
                const std::vector<glm::vec3> &uniformValues) {
  glProgramUniform3fv(programObject, glGetUniformLocation(programObject, uniformName.c_str()),
                      uniformValues.size(), glm::value_ptr(uniformValues[0]));
}

// Matrices
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::mat2 &uniformValue) {
  glProgramUniformMatrix2fv(programObject, glGetUniformLocation(programObject, uniformName.c_str()), 1,
                            GL_FALSE, glm::value_ptr(uniformValue));
}
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::mat3 &uniformValue) {
  glProgramUniformMatrix3fv(programObject, glGetUniformLocation(programObject, uniformName.c_str()), 1,
                            GL_FALSE, glm::value_ptr(uniformValue));
}
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::mat4 &uniformValue) {
  glProgramUniformMatrix4fv(programObject, glGetUniformLocation(programObject, uniformName.c_str()), 1,
                            GL_FALSE, glm::value_ptr(uniformValue));
}