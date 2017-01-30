#include "synchro.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <limits>
#include <system_error>

namespace
{
  typedef std::shared_ptr<void> HandleHolder;
  typedef std::shared_ptr<Utils::Stopper> StopperPtr;

  std::system_error GetSystemError(char const* message)
  {
    DWORD error = GetLastError();
    return std::system_error(error, std::system_category(), "Failed to create semaphore");
  }

  HandleHolder CreateWindowsSemaphore()
  {
    HandleHolder result(::CreateSemaphore(0, 0, LONG_MAX, 0), ::CloseHandle);
    if (!result)
      throw GetSystemError("Failed to create mutex");

    return result;
  }

  HandleHolder CreateManualResetEvent()
  {
    HandleHolder result(::CreateEvent(0, TRUE, FALSE, 0), ::CloseHandle);
    if (!result)
      throw GetSystemError("Failed to create manual reset event");

    return result;
  }

  class MutexImpl : public Utils::Mutex
  {
  public:
    MutexImpl()
    {
      InitializeCriticalSection(&Object);
    }

    ~MutexImpl()
    {
      DeleteCriticalSection(&Object);
    }
    
    virtual void lock()
    {
      EnterCriticalSection(&Object);
    }

    virtual void unlock()
    {
      LeaveCriticalSection(&Object);
    }

  private:
    CRITICAL_SECTION Object;
  };

  class SemaphoreImpl : public Utils::Semaphore
  {
  public:
    SemaphoreImpl(StopperPtr const& stopSigal)
      : Object(CreateWindowsSemaphore())
      , StopSignal(stopSigal)
    {
    }

    virtual void Signal()
    {
      if (!::ReleaseSemaphore(Object.get(), 1, 0))
        throw GetSystemError("Release semaphore failed");
    }

    virtual void Wait()
    {
      HANDLE const objects[] = {Object.get(), StopSignal->GetHandle()};
      DWORD const count = 2;
      DWORD result = WaitForMultipleObjects(count, objects, FALSE, INFINITE);
      if (result == WAIT_OBJECT_0 + count - 1)
        throw std::logic_error("User breaks operation");

      if (result != WAIT_OBJECT_0)
        throw GetSystemError("Wait operation failed");
    }

  private:
    HandleHolder Object;
    StopperPtr StopSignal;
  };

  class StopperImpl : public Utils::Stopper
  {
  public:
    StopperImpl()
      : Object(CreateManualResetEvent())
    {
    }

    virtual void Stop()
    {
      if (!::SetEvent(Object.get()))
        throw GetSystemError("Failed to set event");
    }

    virtual void* GetHandle()
    {
      return Object.get();
    }

  private:
    HandleHolder Object;
  };
}

#if defined CreateMutex
#undef CreateMutex
#endif

#if defined CreateSemaphore
#undef CreateSemaphore
#endif

namespace Utils
{
  std::unique_ptr<Mutex> CreateMutex()
  {
    return std::unique_ptr<Mutex>(new MutexImpl);
  }

  std::unique_ptr<Semaphore> CreateSemaphore(std::shared_ptr<Stopper> const& stopSignal)
  {
    return std::unique_ptr<Semaphore>(new SemaphoreImpl(stopSignal));
  }

  std::shared_ptr<Stopper> CreateStopper()
  {
    return std::shared_ptr<Stopper>(new StopperImpl);
  }

}
