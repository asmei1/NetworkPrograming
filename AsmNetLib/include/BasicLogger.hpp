#pragma once
#include <fstream>
#include <mutex>

#include "ILogger.hpp"

namespace anl
{
   class BasicLogger : public ILogger
   {
   public:
      explicit BasicLogger(const std::string& loggingFormat);
      explicit BasicLogger(const std::string& fileName, const std::string& loggingFormat);
        

      ~BasicLogger() override = default;


      inline std::string levelToStr(const Level& level) const;
      

      void log(const std::string& msg, const Level& level) const override;

   protected:
      std::string parseLog(const std::string& msg, const Level& level) const override;

   private:
      mutable std::mutex mutex;
      std::ofstream logFile;
      bool enableLoggingToFile = true;
   };
}
