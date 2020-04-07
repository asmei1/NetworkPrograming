#pragma once
#include "StoppableTask.hpp"

namespace anl
{
   class ResumableTask : public StoppableTask
   {
   public:

      void resume()
      {
         std::unique_lock<std::mutex> lk(this->pauseMutex);
         this->resumed = true;
         this->pauseCv.notify_one();
      }
      void pause()
      {
         std::unique_lock<std::mutex> lk(this->pauseMutex);
         this->resumed = false;
         this->pauseCv.notify_one();
      }

   protected:
      void waitIfPaused()
      {
         std::unique_lock<std::mutex> lk(this->pauseMutex);
         pauseCv.wait(lk, [this] {return this->resumed.load(); });
      }
   private:
      std::condition_variable pauseCv;
      std::mutex pauseMutex;
      std::atomic_bool resumed = true;

   };
}
