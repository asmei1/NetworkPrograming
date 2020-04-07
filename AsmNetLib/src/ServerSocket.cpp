#include "ServerSocket.h"
#include "ILogger.hpp"
#include "Socket.h"
using namespace anl;

ServerSocket::ServerSocket(ILogger* logger)
{
   this->logger = logger;
}

ServerSocket::~ServerSocket()
{
   if(true == this->initialized)
   {
      closesocket(this->serverSocketHandler);
   }
   /*if(true == this->listening)
   {
      this->listeningThread.join();
   }*/
}

bool ServerSocket::initialize(int portNumber)
{
   this->portNumber = portNumber;
   this->initialized = true;

   int errorCode = 0;
   this->serverSocketHandler = socket(AF_INET, SOCK_STREAM, 0);
   if(INVALID_SOCKET == this->serverSocketHandler)
   {
      errorCode = WSAGetLastError();
      this->logger->error("Could not create a server socket: " + std::to_string(errorCode));
   }
   else
   {
      sockaddr_in server;
      server.sin_family = AF_INET;
      server.sin_addr.s_addr = INADDR_ANY;
      server.sin_port = htons(portNumber);
      if(SOCKET_ERROR == bind(this->serverSocketHandler, reinterpret_cast<sockaddr*>(&server), sizeof(server)))
      {
         errorCode = WSAGetLastError();
         this->logger->error("Bind failed with error code: " + std::to_string(errorCode));
      }
   }

   return errorCode;
}

bool ServerSocket::isReadyForListening() const
{
   return this->initialized;
}

SocketUPtr ServerSocket::waitAndGetClient()
{
   sockaddr_in clientAddrr;
   int size = sizeof(sockaddr_in);
   SOCKET newSocket;


   newSocket = accept(this->serverSocketHandler, reinterpret_cast<sockaddr*>(&clientAddrr), &size);

   //if something went wrong
   if(newSocket == INVALID_SOCKET)
   {
      return nullptr;
   }
   return SocketUPtr(new Socket(this->logger, newSocket, clientAddrr));
}

bool ServerSocket::startListening()
{
   if(SOCKET_ERROR == this->serverSocketHandler)
   {
      throw std::exception{ "Cannot started listening, if server socket was not initialized!" };
   }

   if(false == this->initialized)
   {
      initialize(this->portNumber);
   }


   this->logger->trace("Waiting for incoming client....");


   listen(this->serverSocketHandler, 0);
//   this->listeningThread = std::thread(std::ref(worker));

   return true;
}
void ServerSocket::stopListening()
{
   this->initialized = false;
   closesocket(this->serverSocketHandler);
}

//for later
//
//void ServerSocket::stopListening()
//{
//   this->listening = false;
//   this->initialized = false;
//   closesocket(this->serverSocketHandler);
//   this->worker.stop();
//   this->listeningThread.join();
//}
//
//void ServerSocket::registerClientConnectedHandler(const ClientConnectedHandler& handler)
//{
//   this->clientConnectionHandler = handler;
//}
//
//void ServerSocket::ClientsListeningTask::run()
//{
//   listen(this->socket->serverSocketHandler, 0);
//   sockaddr_in client;
//   int size = sizeof(sockaddr_in);
//   while(true)
//   {
//      SOCKET newSocket;
//      newSocket = accept(this->socket->serverSocketHandler, reinterpret_cast<sockaddr*>(&client), &size);
//
//      //if stopListening method was executed, break the loop
//      if(true == this->stopRequested())
//      {
//         break;
//      }
//
//      //if something went wrong
//      if(newSocket == INVALID_SOCKET)
//      {
//         this->socket->logger->error("Failed to accepted new client: " + std::to_string(WSAGetLastError()));
//         break;
//      }
//
//      //if everything is okey, execute callback function
//      this->socket->clientConnectionHandler(SocketUPtr(new Socket(this->socket->logger, newSocket, client)));
//   }
//}
