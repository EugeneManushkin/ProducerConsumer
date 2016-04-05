#include "request_api.h"
#include "utils.h"

#include <windows.h>

#include <iostream>
#include <memory>

namespace
{
  DWORD const StepTime = 100;
  unsigned const MaxSteps = 50;

  void Step()
  {
    ::Sleep(StepTime);
  }
}

void Stopper::Stop()
{
  InterlockedIncrement(Signal.get());
}

Request* GetRequest(Stopper stopSignal)
{
  unsigned count = Utils::Random(MaxSteps);
  for (unsigned i = 0; i < count; ++i)
  {
    Step();
    if (stopSignal.IsStopped())
    {
      std::cout << "GetRequest stopped\n";
      return 0;
    }
  }

  return new Request;
}

void ProcessRequest(Request* request, Stopper stopSignal)
{
  unsigned count = Utils::Random(MaxSteps);
  for (unsigned i = 0; i < count; ++i)
  {
    Step();
    if (stopSignal.IsStopped())
    {
      std::cout << "ProcessRequest stopped\n";
      return;
    }
  }
}

void DeleteRequest(Request* request)
{
  delete request;
}