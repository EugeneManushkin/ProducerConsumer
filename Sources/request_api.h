#pragma once

#include <memory>

class Request
{
};

class Stopper
{
public:
  Stopper()
    : Signal(new long(0))
  {
  }

  void Stop();

  bool IsStopped()
  {
    return *Signal != 0;
  }

private:
  std::shared_ptr<long volatile> Signal;
};

Request* GetRequest(Stopper stopSignal);
void ProcessRequest(Request* request, Stopper stopSignal);
void DeleteRequest(Request* request);