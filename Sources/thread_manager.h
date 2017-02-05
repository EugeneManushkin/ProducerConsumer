#pragma once

#include <memory>
#include <string>

namespace Utils
{
  class Thread
  {
  public:
    virtual ~Thread()
    {
    }

    void Execute();

  private:
    virtual void ThreadFunc() = 0;
    virtual std::string Name() = 0;
  };

  class ThreadManager
  {
  public:
    virtual ~ThreadManager()
    {
    }

    virtual void AddProducer() = 0;
    virtual void AddConsumer() = 0;
    virtual void Stop() = 0;
  };

  std::unique_ptr<ThreadManager> CreateThreadManager();
}