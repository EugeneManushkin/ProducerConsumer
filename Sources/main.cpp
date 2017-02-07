#include "thread_manager.h"

int main()
{
  unsigned const NumConsumers = 4;
  unsigned const NumProducers = NumConsumers * 2;
  std::unique_ptr<Utils::ThreadManager> manager = Utils::CreateThreadManager(NumProducers, NumConsumers);
  getchar();
  manager->Stop();
  return 0;
}