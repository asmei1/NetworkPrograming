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

   public:
      ~MulticastSocket();
      void closeSocket();

      void initilizeRecv(const InetAddress& addr);
      void sendData(const Data& data, const InetAddress& addr) const;
      void recvData(Data& data) const;
      sockaddr_in getRawSettings() const;

   private:

      void reusePortEnabled();
      void disableMulticastLoop();
      void getLocalInterface(in_addr& localInterface);


      bool receiveInitialized = false;
      ILogger* logger;
      sockaddr_in addrr;
      SOCKET socketHandler;
   };

}
