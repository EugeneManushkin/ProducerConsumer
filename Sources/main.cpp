#include "synchro.h"
#include "thread_manager.h"

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
  unsigned const NumConsumers = 4;
  unsigned const NumProducers = NumConsumers;
  ::SetConsoleCtrlHandler(ConsoleEventHandler, TRUE);
  std::unique_ptr<Utils::ThreadManager> manager = Utils::CreateThreadManager(NumProducers, NumConsumers, GetStopSignal());
  if (!GetStopSignal()->Wait(30 * 1000))
    manager->Stop();

  return 0;
}