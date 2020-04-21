#include "MulticastSocket.h"
#include "ILogger.hpp"
#include "WS2tcpip.h"
#include "Helper.hpp"
#include "InetAddress.h"
#include "Exceptions/TimeoutException.h"
#include "Exceptions/DatagramSizeOutOfRangeException.h"
#include "Exceptions/BindException.h"
#include <cassert>

using namespace anl;

void MulticastSocket::reusePortEnabled()
{
   char reuse = 1;
   if(setsockopt(this->socketHandler, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
   {
      closesocket(this->socketHandler);
      throw WSAGetLastError();
   }
}

void MulticastSocket::disableMulticastLoop()
{
   char loop = 0;
   if(setsockopt(this->socketHandler, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0)
   {
      throw WSAGetLastError();
   }
}

void MulticastSocket::getLocalInterface(in_addr& localInterface)
{
   char myname[32] = { 0 };
   gethostname(myname, 32);
   hostent* he;
   he = gethostbyname(myname);
   memcpy(&localInterface.s_addr, *he->h_addr_list, 4);
}

MulticastSocket::MulticastSocket(ILogger* logger)
{
   this->logger = logger;
   this->socketHandler = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

   if(SOCKET_ERROR == socketHandler)
   {
      throw WSAGetLastError();
   }


   int ttl = 1;
   if(setsockopt(this->socketHandler, IPPROTO_IP, IP_MULTICAST_TTL, reinterpret_cast<char*>(&ttl), sizeof(ttl)) < 0)
   {
      throw WSAGetLastError();
   }

   disableMulticastLoop();
   reusePortEnabled();

   in_addr localInterface;
   getLocalInterface(localInterface);

   if(setsockopt(this->socketHandler, IPPROTO_IP, IP_MULTICAST_IF, reinterpret_cast<char*>(&localInterface), sizeof(localInterface)) < 0)
   {
      throw WSAGetLastError();
   }
}

void MulticastSocket::initilizeRecv(const InetAddress& addr)
{
   this->addrr.sin_family = AF_INET;
   this->addrr.sin_port = htons(addr.getRawSettings().sin_port);
   this->addrr.sin_addr.s_addr = INADDR_ANY;
   if(bind(this->socketHandler, reinterpret_cast<sockaddr*>(&this->addrr), sizeof(this->addrr)))
   {
      throw WSAGetLastError();
   }

   char myname[32] = { 0 };
   gethostname(myname, 32);
   hostent* he;
   he = gethostbyname(myname);

   ip_mreq group;
   group.imr_multiaddr.s_addr = addr.getRawSettings().sin_addr.s_addr;
   memcpy(&group.imr_interface.s_addr, *he->h_addr_list, 4);
   if(setsockopt(this->socketHandler, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<char*>(&group), sizeof(group)) < 0)
   {
      throw WSAGetLastError();
   }

   this->receiveInitialized = true;
}

void MulticastSocket::sendData(const Data& data, const InetAddress& addr) const
{
   if(data.size() > MAX_DATAGRAM_SIZE)
   {
      throw DatagramSizeOutOfRangeException{};
   }

   if(SOCKET_ERROR == sendto(this->socketHandler, data.data(), static_cast<int>(data.size()), 0, addr.toSockAddr(), addr.getStructSize()))
   {
      throw WSAGetLastError();
   }
}

void MulticastSocket::recvData(Data& data) const
{
   assert(this->receiveInitialized);


   if(data.size() > MAX_DATAGRAM_SIZE)
   {
      throw DatagramSizeOutOfRangeException{};
   }

   if(0 > recv(this->socketHandler, data.data(), static_cast<int>(data.size()), 0))
   {
      throw WSAGetLastError();
   }
}

sockaddr_in MulticastSocket::getRawSettings() const
{
   return this->addrr;
}


MulticastSocket::~MulticastSocket()
{
   closeSocket();
}

void MulticastSocket::closeSocket()
{
   closesocket(this->socketHandler);
}
