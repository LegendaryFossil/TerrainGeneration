#include "timeMeasureUtils.h"

#include <cstdio>

static constexpr auto nanoToMilliSeconds = 1000000.0f;

void updateFrameTime(FrameTimeData* frameTimeData) {
  using namespace std::chrono;

  time_point<high_resolution_clock, duration<double>> current = high_resolution_clock::now();
  time_point<high_resolution_clock, duration<double>> epoch(current.time_since_epoch());

  frameTimeData->frameTime = (epoch - frameTimeData->previous).count();
  frameTimeData->previous = current;
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