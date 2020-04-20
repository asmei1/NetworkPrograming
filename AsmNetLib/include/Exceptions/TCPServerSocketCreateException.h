#pragma once
#include <stdexcept>
#include <string>

class TCPServerSocketCreateException : public std::runtime_error
{
   int errorCode;
public:

   explicit TCPServerSocketCreateException(int bindError)
      : runtime_error("Tcp server socket creation failed. Error code: " + std::to_string(bindError))
   {
      this->errorCode = bindError;
   }

   int getErrorCode() const
   {
      return this->errorCode;
   }
};
