#pragma once
#include <exception>

namespace anl
{
   class DatagramSizeOutOfRangeException : std::runtime_error
   {
   public:
      explicit DatagramSizeOutOfRangeException(const std::string& _Message = "Out of range")
         : runtime_error(_Message)
      {
      }
   };
}