#pragma once
#include <winsock2.h>
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include "Types.h"
#include <stdexcept>

namespace anl
{
   class InetAddress;

   class MulticastSocket
   {
      friend class AsmNetwork;
      friend class UDPServerSocket;

      MulticastSocket(ILogger* logger);
      MulticastSocket(ILogger* logger, uint16_t portNumber);

   public:
      ~MulticastSocket();
      void closeSocket();

      void sendData(const Data& data, const InetAddress& addrr) const;
      void recvData(Data& data, const InetAddress& addrr, long timeoutUSec = -1) const;
      InetAddress recvData(Data& data) const;
      sockaddr_in getRawSettings() const;

   private:
      ILogger* logger;
      sockaddr_in addrr;
      SOCKET socketHandler;
   };

}
