#include "timeMeasureUtils.h"

#include <cstdio>

static constexpr auto nanoToMilliSeconds = 1000000.0f;
static constexpr auto milli = 1000000.0f;

void updateFrameTime(FrameTimeData* frameTimeData) {
  using namespace std::chrono;

  const TimePoint newTime = high_resolution_clock::now();
  frameTimeData->frameTimeInSec =
      std::chrono::duration<double>(newTime - frameTimeData->currentTime).count();
  frameTimeData->currentTime = newTime;
}

TimePoint startTimeMeasure() {
  return std::chrono::high_resolution_clock::now();
}

long long endTimeMeasure(const TimePoint& timeStart) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - timeStart).count();
}

void printTime(const std::string message, const long long time) {
  printf("%s: %F milliseconds\n", message.c_str(), time / nanoToMilliSeconds);
}