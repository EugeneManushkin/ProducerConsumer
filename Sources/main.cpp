#include "synchro.h"
#include "thread_manager.h"
#include "utils.h"

#include <windows.h>

namespace
{
  std::shared_ptr<Utils::Stopper> GetStopSignal()
  {
    static std::shared_ptr<Utils::Stopper> stopSignal = Utils::CreateStopper();
    return stopSignal;
  }

  BOOL WINAPI ConsoleEventHandler(DWORD dwCtrlType)
  {
    GetStopSignal()->Stop();
    return TRUE;
  }
}

int main()
{
  Utils::Log("Application started", "main");
  unsigned const NumConsumers = 4;
  unsigned const NumProducers = NumConsumers;
  ::SetConsoleCtrlHandler(ConsoleEventHandler, TRUE);
  std::unique_ptr<Utils::ThreadManager> manager = Utils::CreateThreadManager(NumProducers, NumConsumers, GetStopSignal());
  GetStopSignal()->Wait(30 * 1000);
  manager->Stop();
  Utils::Log("Application stopped", "main");
  return 0;
}