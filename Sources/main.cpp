#include "request_api.h"
#include "utils.h"

#include <windows.h>

#include <iostream>
#include <limits>
#include <thread>
#include <vector>

namespace
{
  void ProducerThread(Utils::GuardedQueue* queue, HANDLE needRequest, HANDLE requestReady, Stopper stop, unsigned number)
  {
    Utils::Log(number, "Producer thread started");
    while (true)
    {
      if (WaitForSingleObject(needRequest, INFINITE) != WAIT_OBJECT_0)
        break;

      if (stop.IsStopped())
      {
        Utils::Log(number, "ProducerThread: stop signal");
        break;
      }

      Request* req = GetRequest(stop);
      if (!req)
      {
        Utils::Log(number, "ProducerThread: user brakes GetRequest");
        break;
      }

      queue->Add(req);
      ReleaseSemaphore(requestReady, 1, 0);
    }

    Utils::Log(number, "ProducerThread: user brakes GetRequest");
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
      if (WaitForSingleObject(requestReady, INFINITE) != WAIT_OBJECT_0)
        break;

      // Print delay if > 20 ms
      measure.Reset(number, 20);
      if (stop.IsStopped())
      {
        Utils::Log(number, "ConsumerThread: stop signal");
        break;
      }

      Request* req = 0;
      if (!queue->Release(req))
        break;

      ReleaseSemaphore(needRequest, 1, 0);
      DoProcessRequest(req, stop, number);
      // Perhaps user canceled request so check IsStopped flag before hard WaitForSingleObject operation
      if (stop.IsStopped())
      {
        Utils::Log(number, "ConsumerThread: stop signal");
        break;
      }
    }

    Utils::Log(number, "Consumer thread stopped");
  }
}

int main()
{
  unsigned const NumProducers = 8;
  unsigned const NumConsumers = 4;
  Stopper stop;
  std::auto_ptr<Utils::GuardedQueue> queue = Utils::CreateQueue();
  HANDLE needRequest = CreateSemaphore(0, NumConsumers * 2, LONG_MAX, 0);
  HANDLE requestReady = CreateSemaphore(0, 0, LONG_MAX, 0);
  std::vector<std::thread> threads;
  for (unsigned i = 0; i < NumProducers; ++i)
    threads.push_back(std::thread(ProducerThread, queue.get(), needRequest, requestReady, stop, i));

  for (unsigned i = 0; i < NumConsumers; ++i)
    threads.push_back(std::thread(ConsumerThread, queue.get(), needRequest, requestReady, stop, i));

  Sleep(30 * 1000);
  stop.Stop();
  ReleaseSemaphore(needRequest, NumProducers, 0);
  ReleaseSemaphore(requestReady, NumConsumers, 0);
  for (std::vector<std::thread>::iterator i = threads.begin(); i != threads.end(); ++i)
    i->join();

  std::cout << "Threads stopped\n";
  CloseHandle(needRequest);
  CloseHandle(requestReady);
  return 0;
}