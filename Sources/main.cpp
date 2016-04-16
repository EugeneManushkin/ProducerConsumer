#include "request_api.h"
#include "utils.h"

#include <windows.h>

#include <iostream>
#include <limits>
#include <thread>
#include <vector>

namespace
{
  bool DoWait(HANDLE object, Stopper stop)
  {
    HANDLE objects[] = {object, stop};
    DWORD count = 2;
    DWORD result = WaitForMultipleObjects(count, objects, FALSE, INFINITE);
    if (result == WAIT_OBJECT_0)
      return false;
      
    // TODO: improve logging
    if (result < WAIT_OBJECT_0 || result >= WAIT_OBJECT_0 + count)
      std::cout << "Error occured\n";
    else
      std::cout << "Stop signaled\n";

    return true;
  }

  void ProducerThread(Utils::GuardedQueue* queue, HANDLE needRequest, HANDLE requestReady, Stopper stop, unsigned number)
  {
    Utils::Log(number, "Producer thread started");
    while (true)
    {
      if (DoWait(needRequest, stop))
        break;

      Request* req = GetRequest(stop);
      if (!req)
      {
        Utils::Log(number, "ProducerThread: user brakes GetRequest");
        break;
      }

      queue->Add(req);
      ReleaseSemaphore(requestReady, 1, 0);
    }

    Utils::Log(number, "Producer thread stopped");
  }

  void DoProcessRequest(Request* req, Stopper stop, unsigned number)
  {
    Utils::Log(number, "Start process request");
    ProcessRequest(req, stop);
    Utils::Log(number, "Stop process request");
    DeleteRequest(req);
  }

  void ConsumerThread(Utils::GuardedQueue* queue, HANDLE needRequest, HANDLE requestReady, Stopper stop, unsigned number)
  {
    Utils::Log(number, "Consumer thread started");
    while (true)
    {
      Utils::MeasureTime measure;
      if (DoWait(requestReady, stop))
        break;

      // Print delay if > 20 ms
      measure.Reset(number, 20);
      Request* req = 0;
      if (!queue->Release(req))
        break;

      ReleaseSemaphore(needRequest, 1, 0);
      DoProcessRequest(req, stop, number);
    }

    Utils::Log(number, "Consumer thread stopped");
  }
}

int main()
{
  unsigned const NumConsumers = 4;
  unsigned const NumProducers = NumConsumers * 2;
  Stopper stop = CreateEvent(0, TRUE, FALSE, 0);
  std::auto_ptr<Utils::GuardedQueue> queue = Utils::CreateQueue();
  HANDLE needRequest = CreateSemaphore(0, NumProducers, LONG_MAX, 0);
  HANDLE requestReady = CreateSemaphore(0, 0, LONG_MAX, 0);
  std::vector<std::thread> threads;
  for (unsigned i = 0; i < NumProducers; ++i)
    threads.push_back(std::thread(ProducerThread, queue.get(), needRequest, requestReady, stop, i));

  for (unsigned i = 0; i < NumConsumers; ++i)
    threads.push_back(std::thread(ConsumerThread, queue.get(), needRequest, requestReady, stop, i));

  Sleep(30 * 1000);
  SetEvent(stop);
  for (std::vector<std::thread>::iterator i = threads.begin(); i != threads.end(); ++i)
    i->join();

  std::cout << "Threads stopped\n";
  CloseHandle(stop);
  CloseHandle(needRequest);
  CloseHandle(requestReady);
  return 0;
}