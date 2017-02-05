#include "queue.h"
#include "request_api.h"
#include "synchro.h"
#include "thread_manager.h"
#include "utils.h"

#include <cassert>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

namespace
{
  class NamedThread : public Utils::Thread
  {
  public:
    NamedThread(std::string const& name)
      : ThreadName(name)
    {
    }

    virtual std::string Name()
    {
      return ThreadName;
    }

  private:
    std::string ThreadName;
  };

  class ProducerThread : public NamedThread
  {
  public:
    ProducerThread(std::string const& name, std::shared_ptr<Utils::Queue> const& queue, std::shared_ptr<Utils::Semaphore> const& needRequest, std::shared_ptr<Utils::Stopper> const& stopSignal)
      : NamedThread(name)
      , Queue(queue)
      , NeedRequest(needRequest)
      , StopSignal(stopSignal)
    {
    }

    virtual void ThreadFunc()
    {
      while (true)
      {
        NeedRequest->Wait();
        std::unique_ptr<Request> req(GetRequest(StopSignal->GetHandle()));
        if (req)
          Queue->Push(std::move(req));
      }
    }

  private:
    std::shared_ptr<Utils::Queue> Queue;
    std::shared_ptr<Utils::Semaphore> NeedRequest;
    std::shared_ptr<Utils::Stopper> StopSignal;
  };

  class ThreadManagerImpl : public Utils::ThreadManager
  {
  public:
    ThreadManagerImpl()
      : StopSignal(Utils::CreateStopper())
      , Queue(Utils::CreateWaitableQueue(StopSignal))
      , NeedRequest(Utils::CreateSemaphore(StopSignal))
      , NumProducers(0)
    {
    }

    ~ThreadManagerImpl()
    {
      assert(Threads.empty());
    }

    virtual void AddProducer()
    {
      std::string name(std::string("Producer") + std::to_string(++NumProducers));
      std::unique_ptr<Utils::Thread> thread(new ProducerThread(name, Queue, NeedRequest, StopSignal));
      Push(std::move(thread));
    }
  
    virtual void AddConsumer()
    {

    }

    virtual void Stop()
    {
      StopSignal->Stop();
      for (auto &i : this->Threads)
        i.join();

      Threads.clear();
    }
  
  private:
    void Push(std::unique_ptr<Utils::Thread> thread)
    {
      Threads.push_back(std::thread(std::bind(&Utils::Thread::Execute, std::move(thread))));
    }

    std::shared_ptr<Utils::Stopper> StopSignal;
    std::shared_ptr<Utils::Queue> Queue;
    std::shared_ptr<Utils::Semaphore> NeedRequest;
    std::vector<std::thread> Threads;
    unsigned NumProducers;
  };
}

namespace Utils
{
  void Thread::Execute()
  {
    Utils::Log("Thread started", Name());
    try
    {
      ThreadFunc();
    }
    catch (std::exception const& e)
    {
      std::cerr << Name() << ": " << e.what();
    }
    
    Utils::Log("Thread stopped", Name());
  }

  std::unique_ptr<ThreadManager> CreateThreadManager()
  {
    return std::unique_ptr<ThreadManager>(new ThreadManagerImpl());
  }
}