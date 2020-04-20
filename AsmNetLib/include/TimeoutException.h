#pragma once
#include <stdexcept>

namespace anl
{
   class TimeoutException : std::runtime_error
   {
   public:
      explicit TimeoutException(const std::string& _Message = "Timeout")
         : runtime_error(_Message)
      {
      }
   };
}
