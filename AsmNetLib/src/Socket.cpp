#include "Socket.h"
#include "ILogger.hpp"
#include "Helper.hpp"
#define RECV_BUFFER_SIZE 512

using namespace anl;

Socket::Socket(ILogger* logger)
{
   this->logger = logger;
}

Socket::Socket(ILogger* logger, SOCKET socketHandler, const sockaddr_in& addrr)
{
   this->logger = logger;
   this->socketHandler = socketHandler;
   this->addrr = addrr;

   //check if still connected

   this->connected = true;
}

int Socket::initialize()
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

bool Socket::connectTo(const std::string& hostName, uint16_t portNumber, uint8_t ipType)
{
   if(true == this->connected)
   {
      closesocket(this->socketHandler);
   }
   if(ipType != AF_INET)
   {
      throw std::exception{ "Socket could not handle a IPv6 address!" };
   }

   bool success = true;

   //ensure, that hostname is a IPv4 address
   const auto& ipOpt = parseAddress(hostName);

   if(true == success)
   {
      this->addrr.sin_addr.s_addr = inet_addr(ipOpt->c_str());
      this->addrr.sin_family = AF_INET;
      this->addrr.sin_port = htons(portNumber);
      success = 0 <= connect(this->socketHandler, reinterpret_cast<sockaddr*>(&this->addrr), sizeof(this->addrr));
   }

   if(false == success)
   {
      this->logger->error("Cannot connect to " + hostName + " on port " + std::to_string(portNumber) + ".\nError code " + std::to_string(WSAGetLastError()));
   }
   else
   {
      this->logger->trace("Connected to " + hostName + " on port " + std::to_string(portNumber));
   }

   this->connected = success;
   return success;
}

bool Socket::isConnected() const
{
   return this->connected;
}

void Socket::closeSocket()
{
   closesocket(this->socketHandler);
}

bool Socket::sendData(const Data& data) const
{
   bool success = true;

   if(false == this->connected)
   {
      throw std::exception{ "Socket is not connected!" };
   }

   if(false == send(this->socketHandler, data.data(), data.size(), 0))
   {
      success = false;
   }

   return success;
}

std::optional<Data> Socket::recvData() const
{
   if(false == this->connected)
   {
      throw std::exception{ "Socket is not connected!" };
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

sockaddr_in Socket::getRawSettings() const
{
   return this->addrr;
}

Socket::~Socket()
{
   if(true == this->connected)
   {
      closeSocket();
   }
}
