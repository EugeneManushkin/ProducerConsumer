#include "queue.h"
#include "request_api.h"
#include "synchro.h"

#include <deque>
#include <mutex>

namespace
{
  typedef std::lock_guard<Utils::Mutex> MutexLock;
  typedef std::shared_ptr<Request> RequestPtr;

  class GuardedQueue : public Utils::Queue
  {
  public:
    GuardedQueue()
      : Guard(Utils::CreateMutex())
    {
    }

    virtual RequestPtr Pop()
    {
      RequestPtr result;
      MutexLock lock(*Guard);
      if (Requests.empty())
        return result;

      result = Requests.back();
      Requests.pop_back();
      return result;
    }

    virtual void Push(RequestPtr req)
    {
      MutexLock lock(*Guard);
      Requests.push_front(req);
    }

  private:
    std::deque<RequestPtr> Requests;
    std::unique_ptr<Utils::Mutex> Guard;
  };

  class WaitableQueue : public Utils::Queue
  {
  public:
    WaitableQueue(std::shared_ptr<Utils::Stopper> const& stopSignal)
      : RequestReady(Utils::CreateSemaphore(stopSignal))
    {
    }

    virtual RequestPtr Pop()
    {
      RequestReady->Wait();
      RequestPtr result(Requests.Pop());
      if (!result)
        throw std::logic_error("Request queue is empty");

      return result;
    }

    virtual void Push(RequestPtr req)
    {
      Requests.Push(std::move(req));
      RequestReady->Signal();
    }

  private:
    GuardedQueue Requests;
    std::unique_ptr<Utils::Semaphore> RequestReady;
  };
}

namespace Utils
{
  std::unique_ptr<Queue> CreateWaitableQueue(std::shared_ptr<Stopper> const& stopSignal)
  {
    return std::unique_ptr<Queue>(new WaitableQueue(stopSignal));
  }
}