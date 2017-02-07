#include "request_api.h"
#include "utils.h"

#include <windows.h>

#include <memory>
#include <sstream>

namespace
{
  DWORD GetRandomTimeout()
  {
    return 1000 * Utils::Random(5);
  }

  void LogPointerOperation(char const* operation, void* ptr)
  {
    std::stringstream stream;
    stream << operation << ": " << ptr;
    Utils::Log(stream.str(), "---------");
  }
}

Request* GetRequest(Stopper stopSignal)
{  
  Request* ptr = 0;
  if (WaitForSingleObject(stopSignal, GetRandomTimeout()) != WAIT_OBJECT_0)
    ptr = new Request;

  LogPointerOperation("allocated", ptr);
  return ptr; 
}

void ProcessRequest(Request* request, Stopper stopSignal)
{
  if (WaitForSingleObject(stopSignal, GetRandomTimeout()) == WAIT_OBJECT_0)
    LogPointerOperation("processing interrupted", request);
  else
    LogPointerOperation("processed", request);
}

void DeleteRequest(Request* request)
{
  LogPointerOperation("freed", request);
  delete request;
}