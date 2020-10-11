#include "utils.h"

#include <Windows.h>

std::string getExePath() {
  char filePath[MAX_PATH] = {0};
  GetModuleFileName(NULL, filePath, MAX_PATH);
  std::string filePathStr(filePath);
  filePathStr = filePathStr.substr(0, filePathStr.find_last_of("\\/"));
  return filePathStr;
}
