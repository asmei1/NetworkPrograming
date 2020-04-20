#include "TCPSocket.h"
#include "ILogger.hpp"
#include "Helper.hpp"
#include "InetAddress.h"
#define RECV_BUFFER_SIZE 512

using namespace anl;

TCPSocket::TCPSocket(ILogger* logger)
{
   this->logger = logger;
}

TCPSocket::TCPSocket(ILogger* logger, SOCKET socketHandler, const sockaddr_in& addrr)
{
   this->logger = logger;
   this->socketHandler = socketHandler;
   this->addrr = addrr;

   //check if still connected

   this->connected = true;
}

int TCPSocket::initialize()
{
   int errorCode = 0;
   this->socketHandler = socket(AF_INET, SOCK_STREAM, 0);
   if(INVALID_SOCKET == socketHandler)
   {
      errorCode = WSAGetLastError();
      this->logger->error("Could not create a socket: " + std::to_string(errorCode));
   }

   return errorCode;
}

void TCPSocket::connectTo(const InetAddress& address)
{
   if(true == this->connected)
   {
      closesocket(this->socketHandler);
   }

   bool success = 0 <= connect(this->socketHandler, address.toSockAddr(), address.getStructSize());

   if(false == success)
   {
      throw WSAGetLastError();
   }

   this->connected = success;
}

bool TCPSocket::isConnected() const
{
   return this->connected;
}

void TCPSocket::closeSocket()
{
   closesocket(this->socketHandler);
}

bool TCPSocket::sendData(const Data& data) const
{
   bool success = true;

   if(false == this->connected)
   {
      throw std::exception{ "TCPSocket is not connected!" };
   }

   if(false == send(this->socketHandler, data.data(), data.size(), 0))
   {
      success = false;
   }

   return success;
}

std::optional<Data> TCPSocket::recvData() const
{
   if(false == this->connected)
   {
      throw std::exception{ "TCPSocket is not connected!" };
   }

   Data recvData;
   int receivedBytes;
   do
   {
      char buff[RECV_BUFFER_SIZE];
      receivedBytes = recv(this->socketHandler, buff, RECV_BUFFER_SIZE, 0);
      if(receivedBytes == SOCKET_ERROR || receivedBytes == 0)
      {
         return std::nullopt;
      }
      recvData.insert(recvData.end(), buff, buff + receivedBytes);

   } while(receivedBytes >= RECV_BUFFER_SIZE);

   return recvData;
}

sockaddr_in TCPSocket::getRawSettings() const
{
   return this->addrr;
}

TCPSocket::~TCPSocket()
{
   if(true == this->connected)
   {
      closeSocket();
   }
}
