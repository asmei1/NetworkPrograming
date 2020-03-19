#pragma once
#include <winsock2.h>
#include <memory>
#include "ILogger.hpp"

namespace anl
{
   class Socket;
   using SocketUPtr = std::unique_ptr<Socket>;

   class AsmNetwork
   {
   public:
      static bool initialize(ILogger* loggerInstance);
      static int cleanup();
      static ILogger* getLogger();

      static SocketUPtr createSocket();

   private:
      static inline WSADATA wsa;
      static inline ILogger* logger;
   };
}

