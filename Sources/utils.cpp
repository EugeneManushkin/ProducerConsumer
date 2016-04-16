#include "request_api.h"
#include "utils.h"

#include <windows.h>

#include <deque>
#include <iostream>
#include <random>
#include <stdarg.h>
#include <string>
#include <time.h>

namespace
{
  struct Locker
  {
    Locker(CRITICAL_SECTION* mutex)
      : Mutex(mutex)
    {
      EnterCriticalSection(Mutex);
    }

    ~Locker()
    {
      LeaveCriticalSection(Mutex);
    }

  private:
    CRITICAL_SECTION* Mutex;
  };

  class GuardedQueueImpl : public Utils::GuardedQueue
  {
  public:
    GuardedQueueImpl()
    {
      InitializeCriticalSection(&Mutex);
    }

    ~GuardedQueueImpl()
    {
      DeleteCriticalSection(&Mutex);
      for (std::deque<Request*>::const_iterator i = Requests.begin(); i != Requests.end(); ++i)
        DeleteRequest(*i);
    }

    bool Release(Request*& result)
    {
      Locker guard(&Mutex);
      if (Requests.empty())
        return 0;

      result = Requests.back();
      Requests.pop_back();
      return true;
    }

    void Add(Request* req)
    {
      Locker guard(&Mutex);
      Requests.push_front(req);
    }

  private:
    std::deque<Request*> Requests;
    CRITICAL_SECTION Mutex;
  };

  class Randomizer
  {
  public:
    unsigned Random(unsigned max)
    {
      Locker guard(&Mutex);
      std::uniform_int_distribution<unsigned> d(1, max);
      return d(Engine);
    }
 
    Randomizer()
      : Engine(std::random_device()())
    {
      InitializeCriticalSection(&Mutex);
    }

    ~Randomizer()
    {
      DeleteCriticalSection(&Mutex);
    }

  private:
    std::mt19937 Engine;
    CRITICAL_SECTION Mutex;
  };

  static Randomizer Rand;
}

namespace Utils
{
  MeasureTime::MeasureTime()
    : Start(GetTickCount())
  {
  }

  void MeasureTime::Reset(unsigned alarmTimeout, std::string const& threadName)
  {
    DWORD prev = Start;
    Start = GetTickCount();
    if (Start - prev <= alarmTimeout)
      return;

    std::string str(std::to_string(Start - prev));
    str = std::string("Delay = ") + str;
    Log(str, threadName);
  }

  std::auto_ptr<GuardedQueue> CreateQueue()
  {
    return std::auto_ptr<GuardedQueue>(new GuardedQueueImpl);
  }

  void Log(std::string const& message, std::string const& threadName)
  {
    char buf[256];
    sprintf_s(buf, "%05u ", GetTickCount() % (60 * 1000));
    std::string s(std::string(buf) + threadName + std::string(": ") + message + std::string("\n"));
    std::cout << s.c_str();
  }

  unsigned Random(unsigned max)
  {
    return Rand.Random(max);
  }
}