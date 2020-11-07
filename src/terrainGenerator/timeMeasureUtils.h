#pragma once

#include <chrono>
#include <string>

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>>;

struct FrameTimeData {
  TimePoint currentTime = std::chrono::high_resolution_clock::now();
  double frameTimeInSec;
};

void updateFrameTime(FrameTimeData* frameTimeData);

TimePoint startTimeMeasure();
long long endTimeMeasure(const TimePoint& timeStart);
void printTime(const std::string message, const long long time);

