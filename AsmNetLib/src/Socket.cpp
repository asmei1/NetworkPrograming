#include "Socket.h"
#include "AsmNetwork.h"
#define RECV_BUFFER_SIZE 512

using namespace anl;

Socket::Socket()
{
}

int Socket::initialize()
{
   int errorCode = 0;
   this->socketHandler = socket(AF_INET, SOCK_STREAM, 0);
   if(INVALID_SOCKET == socketHandler)
   {
      errorCode = WSAGetLastError();
      AsmNetwork::getLogger()->error("Could not create a socket: " + std::to_string(errorCode));
   }

   return errorCode;
}

bool Socket::connectTo(const std::string& hostName, uint16_t portNumber, uint8_t ipType)
{
   if(true == this->connected)
   {
      closesocket(this->socketHandler);
   }

   bool success = true;

   sockaddr_in destinationAddress;

   if(ipType != AF_INET)
   {
      throw std::exception{ "Socket could not handle a IPv6 address!" };
   }

   destinationAddress.sin_addr.s_addr = inet_addr(hostName.c_str());
   destinationAddress.sin_family = AF_INET;
   destinationAddress.sin_port = htons(portNumber);

   if(0 > connect(this->socketHandler, reinterpret_cast<sockaddr*>(&destinationAddress), sizeof(destinationAddress)))
   {
      AsmNetwork::getLogger()->error("Cannot connect to " + hostName + " on port " + std::to_string(portNumber) + ".\nError code " + std::to_string(WSAGetLastError()));
      success = false;
   }
   else
   {
      AsmNetwork::getLogger()->trace("Connected to " + hostName + " on port " + std::to_string(portNumber));
   }

   this->connected = success;
   return success;
}

bool Socket::isConnected() const
{
   return this->connected;
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
      if(receivedBytes == -1)
      {
         return std::nullopt;
      }
      recvData.insert(recvData.end(), buff, buff + receivedBytes);
      
   } while(receivedBytes > RECV_BUFFER_SIZE);

   return recvData;
}

Socket::~Socket()
{
   if(true == this->connected)
   {
      closesocket(this->socketHandler);
   }
}
