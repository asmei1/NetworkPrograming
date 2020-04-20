#pragma once
#include <winsock2.h>
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include "Types.h"

namespace anl
{
   class InetAddress;

   class TCPSocket
   {
      friend class AsmNetwork;
      friend class TCPServerSocket;
      TCPSocket(ILogger* logger);
      TCPSocket(ILogger* logger, SOCKET socketHandler, const sockaddr_in& addrr);
   public:
      ~TCPSocket();
      int initialize();
      void connectTo(const InetAddress& address);
      bool isConnected() const;
      void closeSocket();

      bool sendData(const Data& data) const;
      std::optional<Data> recvData() const;
      sockaddr_in getRawSettings() const;
   private:
      ILogger* logger;
      sockaddr_in addrr;
      SOCKET socketHandler;
      bool connected = false;
   };

}
