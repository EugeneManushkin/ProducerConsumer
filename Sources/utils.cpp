#include "request_api.h"
#include "utils.h"

#include <windows.h>

#include <deque>
#include <iostream>
#include <stdlib.h>
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
        delete *i;
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
      return rand() % max + 1;
    }
 
    Randomizer()
    {
      srand(static_cast<unsigned>(time(0)));
      InitializeCriticalSection(&Mutex);
    }

    ~Randomizer()
    {
      DeleteCriticalSection(&Mutex);
    }

  private:
    CRITICAL_SECTION Mutex;
  };

  Randomizer Rand;
}

namespace Utils
{
  MeasureTime::MeasureTime()
    : Start(GetTickCount())
  {
  }

  void MeasureTime::Reset(unsigned numThread, unsigned alarmTimeout)
  {
    DWORD prev = Start;
    Start = GetTickCount();
    if (Start - prev <= alarmTimeout)
      return;

    std::string str(std::to_string(Start - prev));
    str = std::string("Delay = ") + str;
    Log(numThread, str);
  }

  std::auto_ptr<GuardedQueue> CreateQueue()
  {
    return std::auto_ptr<GuardedQueue>(new GuardedQueueImpl);
  }

  void Log(unsigned threadNum, std::string const& str)
  {
    std::string s(std::to_string(threadNum));
    s += std::string(": ") + str + std::string("\n");
    std::cout << s.c_str();
  }

  unsigned Random(unsigned max)
  {
    return Rand.Random(max);
  }
}