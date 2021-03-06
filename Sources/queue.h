#pragma once

#include <memory>

class Request;

namespace Utils
{
  class Stopper;

  class Queue
  {
  public:
    virtual ~Queue()
    {
    }

    virtual std::shared_ptr<Request> Pop() = 0;
    virtual void Push(std::shared_ptr<Request>) = 0;
  };

  std::unique_ptr<Queue> CreateWaitableQueue(std::shared_ptr<Stopper> const& stopSignal);
}