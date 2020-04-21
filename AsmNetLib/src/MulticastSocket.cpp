#include "MulticastSocket.h"
#include "ILogger.hpp"
#include "Helper.hpp"
#include "InetAddress.h"
#include "Exceptions/TimeoutException.h"
#include "Exceptions/DatagramSizeOutOfRangeException.h"
#include "Exceptions/BindException.h"

using namespace anl;


MulticastSocket::MulticastSocket(ILogger* logger)
{
   this->logger = logger;
   this->socketHandler = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

   if(SOCKET_ERROR == socketHandler)
   {
      throw WSAGetLastError();
   }

}

MulticastSocket::MulticastSocket(ILogger* logger, uint16_t portNumber)
{
   this->logger = logger;


   this->socketHandler = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

   if(SOCKET_ERROR == socketHandler)
   {
      throw WSAGetLastError();
   }

   this->addrr.sin_addr.S_un.S_addr = INADDR_ANY;
   this->addrr.sin_family = AF_INET;
   this->addrr.sin_port = htons(portNumber);

   //Bind
   bind(this->socketHandler, (sockaddr*)&this->addrr, sizeof(this->addrr));
   if(SOCKET_ERROR == this->socketHandler)
   {
      throw BindException(WSAGetLastError());
   }
}

void MulticastSocket::sendData(const Data& data, const InetAddress& addrr) const
{
   if(data.size() > MAX_DATAGRAM_SIZE)
   {
      throw DatagramSizeOutOfRangeException{};
   }

   if(SOCKET_ERROR == sendto(this->socketHandler, data.data(), static_cast<int>(data.size()), 0, addrr.toSockAddr(), addrr.getStructSize()))
   {
      throw WSAGetLastError();
   }
}

void MulticastSocket::recvData(Data& data, const InetAddress& addrr, long timeoutUSec) const
{
   if(data.size() > MAX_DATAGRAM_SIZE)
   {
      throw DatagramSizeOutOfRangeException{};
   }

   if(timeoutUSec != -1)
   {
      fd_set fds;
      int n;
      struct timeval tv;

      // Set up the file descriptor set.
      FD_ZERO(&fds);
      FD_SET(this->socketHandler, &fds);

      // Set up the struct timeval for the timeout.
      tv.tv_sec = 0;
      tv.tv_usec = timeoutUSec;

      // Wait until timeout or data received.
      n = select(this->socketHandler, &fds, NULL, NULL, &tv);
      if(n == 0)
      {
         throw TimeoutException{};
      }
      else if(n == -1)
      {
         throw WSAGetLastError();
      }
   }

   int size = addrr.getStructSize();
   if(SOCKET_ERROR == recvfrom(this->socketHandler, data.data(), static_cast<int>(data.size()), 0, addrr.toSockAddr(), &size))
   {
      throw WSAGetLastError();
   }
}

InetAddress MulticastSocket::recvData(Data& data) const
{
   if(data.size() > MAX_DATAGRAM_SIZE)
   {
      throw DatagramSizeOutOfRangeException{};
   }

   
   if(data.size() == 0)
   {
      data.resize(MAX_DATAGRAM_SIZE);
   }
   sockaddr_in tempAddress;
   int size = sizeof(tempAddress);
   if(SOCKET_ERROR == recvfrom(this->socketHandler, data.data(), static_cast<int>(data.size()), 0, reinterpret_cast<sockaddr*>(&tempAddress), &size))
   {
      throw WSAGetLastError();
   }

   return InetAddress(tempAddress);
}

sockaddr_in MulticastSocket::getRawSettings() const
{
   return this->addrr;
}

MulticastSocket::~MulticastSocket()
{
   closeSocket();
}
