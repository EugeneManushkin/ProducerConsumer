#include "synchro.h"
#include "utils.h"

#include <iostream>
#include <random>
#include <thread>
#include <sstream>
#include <iomanip>
#include <mutex>

namespace
{
  class Randomizer
  {
  public:
    unsigned Random(unsigned max)
    {
      std::lock_guard<Utils::Mutex> guard(*Mutex);
      std::uniform_int_distribution<unsigned> d(1, max);
      return d(Engine);
    }
 
    Randomizer()
      : Engine(std::random_device()())
      , Mutex(Utils::CreateMutex())
    {
    }

  private:
    std::mt19937 Engine;
    std::unique_ptr<Utils::Mutex> Mutex;
  };

  static Randomizer Rand;

  typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;
}

namespace Utils
{
  MeasureTime::MeasureTime()
    : Start(std::chrono::system_clock::now())
  {
  }

  void MeasureTime::Reset(unsigned alarmTimeout, std::string const& threadName)
  {
    typedef std::chrono::duration<double> DurationInSeconds;
    TimePoint prev = Start;
    Start = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(Start - prev);
    if (duration.count() <= alarmTimeout)
      return;

    std::string str(std::to_string(duration.count()));
    str = std::string("Delay = ") + str;
    Log(str, threadName);
  }

  void Log(std::string const& message, std::string const& threadName)
  {
    std::stringstream stream;   
    stream << std::setw(5) << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % (60 * 1000) << " T:" << std::setw(10) << std::this_thread::get_id() << " " << threadName << ": " << message << std::endl;
    std::cout << stream.str();
  }

  unsigned Random(unsigned max)
  {
    return Rand.Random(max);
  }
}