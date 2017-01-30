#pragma once

#include <memory>

namespace Utils
{
  class Mutex
  {
  public:
    virtual ~Mutex()
    {
    }

    virtual void lock() = 0;
    virtual void unlock() = 0;
  };

  class Semaphore
  {
  public:
    virtual ~Semaphore()
    {
    }

    virtual void Signal() = 0;
    virtual void Wait() = 0;
  };

  class Stopper
  {
  public:
    virtual ~Stopper()
    {
    }

    virtual void Stop() = 0;
    virtual void* GetHandle() = 0;
  };

  std::unique_ptr<Mutex> CreateMutex();
  std::unique_ptr<Semaphore> CreateSemaphore(std::shared_ptr<Stopper> const& stopSignal);
  std::shared_ptr<Stopper> CreateStopper();
}
