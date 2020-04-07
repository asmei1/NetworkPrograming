#pragma once
#include <winsock2.h>
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include "Types.h"

namespace anl
{
   class Socket
   {
      friend class AsmNetwork;
      friend class ServerSocket;
      Socket(ILogger* logger);
      Socket(ILogger* logger, SOCKET socketHandler, const sockaddr_in& addrr);
   public:
      ~Socket();
      int initialize();
      bool connectTo(const std::string& hostName, uint16_t portNumber, uint8_t ipType = AF_INET);
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
