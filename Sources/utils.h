#pragma once

#include <chrono>
#include <memory>
#include <string>

namespace Utils
{
  class MeasureTime
  {
  public:
    MeasureTime();
    void Reset(unsigned alarmTimeout, std::string const& threadName);

  private:
    //TODO: rework
    typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;
    TimePoint Start;
  };

  void Log(std::string const& message, std::string const& threadName);
  unsigned Random(unsigned max);
}