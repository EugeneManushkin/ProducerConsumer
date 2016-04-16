#pragma once

#include <memory>
#include <string>

namespace Utils
{
  class GuardedQueue
  {
  public:
    virtual ~GuardedQueue()
    {
    }

    virtual bool Release(Request*& result) = 0;
    virtual void Add(Request* req) = 0;
  };

  class MeasureTime
  {
  public:
    MeasureTime();
    void Reset(unsigned alarmTimeout, std::string const& threadName);

  private:
    // Assumed Microsoft compiler is used so sizeof(Start) == sizeof(DWORD)
    unsigned Start;
  };

  std::auto_ptr<GuardedQueue> CreateQueue();
  void Log(std::string const& message, std::string const& threadName);
  unsigned Random(unsigned max);
}