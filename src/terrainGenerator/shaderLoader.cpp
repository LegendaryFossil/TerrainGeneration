#include "ShaderLoader.h"

#include "windows.h"

namespace {
  std::string getExePath() {
    char filePath[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, filePath, MAX_PATH);
    std::string filePathStr(filePath);
    filePathStr = filePathStr.substr(0, filePathStr.find_last_of("\\/"));
    return filePathStr;
  }

  const std::string shaderPath(getExePath() + "/resources/shaders/");

  GLuint createShaderObject(GLenum eShaderType, const std::string& shaderName) {
    GLuint shaderObject = glCreateShader(eShaderType);
    std::ifstream shaderFile(shaderName);
    if (shaderFile.is_open())
    {
      std::string content((std::istreambuf_iterator<char>(shaderFile.rdbuf())), (std::istreambuf_iterator<char>()));
      const char* shaderContent = content.c_str();
      glShaderSource(shaderObject, 1, &shaderContent, NULL);
      glCompileShader(shaderObject);

      GLint status;
      glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
      if (status == GL_FALSE)
      {
        GLint infoLogLength;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* logInfo = new char[infoLogLength + 1];
        glGetShaderInfoLog(shaderObject, infoLogLength, NULL, logInfo);

        const char* strShaderType = NULL;
        switch (eShaderType)
        {
        case GL_VERTEX_SHADER: strShaderType = "vertex";
          break;

        case GL_GEOMETRY_SHADER: strShaderType = "geometry";
          break;

        case GL_FRAGMENT_SHADER: strShaderType = "fragment";
          break;
        }

        fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, logInfo);
        delete[] logInfo;
      }

      return shaderObject;
    }
    else
    {
      std::cout << "Unable to open shader file: " << shaderName << std::endl;
      exit(1);
    }
  }

  GLuint createProgramObject(GLuint vertexShaderObject, GLuint fragmentShaderObject) {
    GLuint programObject = glCreateProgram();
    glAttachShader(programObject, vertexShaderObject);
    glAttachShader(programObject, fragmentShaderObject);
    glLinkProgram(programObject);

    GLint status;
    glGetProgramiv(programObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
      GLint infoLogLength;
      glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLogLength);
      GLchar* logInfo = new char[infoLogLength + 1];
      glGetProgramInfoLog(programObject, infoLogLength, NULL, logInfo);
      fprintf(stderr, "Linker failure: %s\n", logInfo);
      delete[] logInfo;
    }

    glDetachShader(programObject, vertexShaderObject);
    glDetachShader(programObject, fragmentShaderObject);

    return programObject;
  }
}


GLuint loadShaders(const std::string& vertexShaderName, const std::string& fragmentShaderName)
{
  GLuint vertexShaderObject = createShaderObject(GL_VERTEX_SHADER, shaderPath + vertexShaderName);
  GLuint fragmentShaderObject = createShaderObject(GL_FRAGMENT_SHADER, shaderPath + fragmentShaderName);

  GLuint programObject = createProgramObject(vertexShaderObject, fragmentShaderObject);

  return programObject;
}

void deleteProgramObject(GLuint programObject)
{
  glDeleteProgram(programObject);
}