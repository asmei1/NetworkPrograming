#pragma once
#include <stdexcept>
#include <string>

class BindException : public std::runtime_error
{
   int errorCode;
public:

   explicit BindException(int bindError)
      : runtime_error("Bind failed. Error code: " + std::to_string(bindError))
   {
      this->errorCode = bindError;
   }

   int getErrorCode() const
   {
      return this->errorCode;
   }
};
