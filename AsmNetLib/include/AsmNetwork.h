#pragma once
#include <winsock2.h>
#include <memory>
#include "Types.h"

namespace anl
{
   class AsmNetwork
   {
   public:
      static bool initialize(ILogger* loggerInstance);
      static int cleanup();
      static ILogger* getLogger();

      static UDPSocketUPtr createUDPSocket();
      static UDPSocketUPtr createUDPSocket(uint16_t portNumber);

      static TCPSocketUPtr createTCPSocket();
      static TCPServerSocketUPtr createTCPServerSocket();

   private:
      static inline WSADATA wsa;
      static inline ILogger* logger;
   };
}

