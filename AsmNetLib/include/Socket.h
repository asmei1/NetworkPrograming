#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string>
#include <vector>
#include <functional>
#include <optional>

namespace anl
{
   using Data = std::vector<char>;


   class Socket
   {
      friend class AsmNetwork;
      Socket();
   public:
      ~Socket();

      int initialize();
      bool connectTo(const std::string& hostName, uint16_t portNumber, uint8_t ipType = AF_INET);
      bool isConnected() const;

      bool sendData(const Data& data) const;
      std::optional<Data> recvData() const;
   private:
      SOCKET socketHandler;
      bool connected = false;
   };

}
