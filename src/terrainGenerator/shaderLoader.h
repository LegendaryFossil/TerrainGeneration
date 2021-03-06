#pragma once

#include "GL\glew.h"
#include "glm\glm.hpp"
#include <string>
#include <vector>

GLuint compileShader(const std::string &shaderFileName, GLuint shaderType);
GLuint createProgramObject(const std::vector<GLuint> &shaderObjects);
void validateProgramObject(const GLuint programObject);
void deleteProgramObject(GLuint programObject);

// Int
void setUniform(const GLuint programObject, const std::string &uniformName, const int uniformValue);
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::ivec2 &uniformValue);
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::ivec3 &uniformValue);
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::ivec4 &uniformValue);

// Float
void setUniform(const GLuint programObject, const std::string &uniformName, const float uniformValue);
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::vec2 &uniformValue);
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::vec3 &uniformValue);
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::vec4 &uniformValue);

// Float arrays
void setUniform(const GLuint programObject, const std::string &uniformName,
                const std::vector<float> &uniformValues);
void setUniform(const GLuint programObject, const std::string &uniformName,
                const std::vector<glm::vec3> &uniformValues);
void setUniform(const GLuint programObject, const std::string &uniformName,
                const std::vector<glm::vec4> &uniformValues);

// Matrices
void setUniform(const GLuint programObject, const std::string &uniformName, const float uniformValue);
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::mat2 &uniformValue);
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::mat3 &uniformValue);
void setUniform(const GLuint programObject, const std::string &uniformName, const glm::mat4 &uniformValue);