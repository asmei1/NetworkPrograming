#include "BasicLogger.hpp"
#include <thread>
#include <sstream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <cassert>
#include "Helper.hpp"

using namespace anl;

BasicLogger::BasicLogger(const std::string& loggingFormat)
   : ILogger(loggingFormat), enableLoggingToFile(false)
{}

BasicLogger::BasicLogger(const std::string& fileName, const std::string& loggingFormat)
   : ILogger(loggingFormat)
{
   this->logFile.open(fileName, std::ofstream::app);
}

std::string BasicLogger::levelToStr(const Level& level) const
{
   switch(level)
   {
      case Level::INFO: return "Info";
      case Level::WARNING: return "Warning";
      case Level::ERR: return "Error";
      case Level::DEBUG: return "Debug";
      case Level::TRACE: return "Trace";
      default: assert(false);
   }
   return "";
}

void BasicLogger::log(const std::string& msg, const Level& level) const
{
   std::lock_guard<std::mutex> lock(this->mutex);


   std::cout << parseLog(msg, level) << std::endl;
}

std::string BasicLogger::parseLog(const std::string& msg, const Level& level) const
{
   std::string output = this->format;
   anl::replace(output, "%LOG_LEVEL%", levelToStr(level));

   const auto threadIdPos = output.find("%THREAD_ID%");
   if(std::string::npos != threadIdPos)
   {
      std::stringstream ss;
      ss << std::this_thread::get_id();
      anl::replace(output, "%THREAD_ID%", ss.str());
   }

   const auto timePos = output.find("%TIME%");
   if(std::string::npos != timePos)
   {
      std::time_t timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      char timedisplay[100];
      struct tm buf;
      errno_t err = localtime_s(&buf, &timenow);
      if(std::strftime(timedisplay, sizeof(timedisplay), this->dateTimeFormat.c_str(), &buf))
      {
         anl::replace(output, "%TIME%", timedisplay);
      }
   }

   output += msg;
   return output;
}

