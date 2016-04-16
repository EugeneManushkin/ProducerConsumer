#include "request_api.h"
#include "utils.h"

#include <windows.h>

#include <iostream>
#include <memory>

namespace
{
  DWORD GetRandomTimeout()
  {
    return 1000 * Utils::Random(5);
  }
}

Request* GetRequest(Stopper stopSignal)
{  
  if (WaitForSingleObject(stopSignal, GetRandomTimeout()) != WAIT_OBJECT_0)
    return new Request;

  return 0; 
}

void ProcessRequest(Request* request, Stopper stopSignal)
{
  if (WaitForSingleObject(stopSignal, GetRandomTimeout()) == WAIT_OBJECT_0)
    Utils::Log("ProcessRequest stopped", "---------");
}

void DeleteRequest(Request* request)
{
  delete request;
}