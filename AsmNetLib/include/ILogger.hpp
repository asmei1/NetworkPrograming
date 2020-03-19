#pragma once
#include <string>
#include <array>

namespace anl
{
   class ILogger
   {
   public:
      /**
       * \brief Initialize logger and set log format message. Allowed tags: %TIME%, %LOG_LEVEL%, %THREAD_ID%.
       * \param loggingFormat Format of logging message. If loggingFormat will be empty, log message will be logged without any additional info.
       *                      Example: [%TIME%] [%LOG_LEVEL%] [%THREAD_ID%]
       */
      ILogger(const std::string& loggingFormat)
         : format(loggingFormat)
      {
         for(uint8_t i = 0; i < LOG_LEVELS_COUNT; ++i)
         {
            this->levelsLoggingPermission[i] = true;
         }
      }

      virtual ~ILogger(){};

      enum class Level : uint8_t
      {
         INFO = 0,
         WARNING,
         ERR,
         DEBUG,
         TRACE,
      };


      virtual void log(const std::string& msg, const Level& level) const = 0;


      void enableLoggingForLevel(const Level& level, bool enable)
      {
         this->levelsLoggingPermission[static_cast<int>(level)] = enable;
      }

      //TODO Add function description
      void setDateTimeFormat(const std::string& dateTimeFormat)
      {
         this->dateTimeFormat = dateTimeFormat;
      }

      void info(const std::string& msg)
      {
         if(true == this->levelsLoggingPermission[static_cast<int>(ILogger::Level::INFO)])
         {
            log(msg, ILogger::Level::INFO);
         }
      }

      void warning(const std::string& msg)
      {
         if(true == this->levelsLoggingPermission[static_cast<int>(ILogger::Level::WARNING)])
         {
            log(msg, ILogger::Level::WARNING);
         }
      }

      void error(const std::string& msg)
      {
         if(true == this->levelsLoggingPermission[static_cast<int>(ILogger::Level::ERR)])
         {
            log(msg, ILogger::Level::ERR);
         }
      }

      void debug(const std::string& msg)
      {
         if(true == this->levelsLoggingPermission[static_cast<int>(ILogger::Level::DEBUG)])
         {
            log(msg, ILogger::Level::DEBUG);
         }
      }

      void trace(const std::string& msg)
      {
         if(true == this->levelsLoggingPermission[static_cast<int>(ILogger::Level::TRACE)])
         {
            log(msg, ILogger::Level::TRACE);
         }
      }

   protected:
      virtual std::string parseLog(const std::string& msg, const Level& level) const = 0;

      const std::string format;
      std::string dateTimeFormat = "%H:%M:%S";

   private:
      constexpr static inline uint8_t LOG_LEVELS_COUNT = 5;
      std::array<bool, LOG_LEVELS_COUNT> levelsLoggingPermission;
   };
}
