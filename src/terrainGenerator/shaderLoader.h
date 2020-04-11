#pragma once

#include "GL\glew.h"

#include <string>
#include <fstream>
#include <iostream>
#include <iterator>

GLuint loadShaders(const std::string& vertexShaderName, const std::string& fragmentShaderName);
void deleteProgramObject(GLuint programObject);