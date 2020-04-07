#include "ServerSocket.h"
#include "ILogger.hpp"
#include "Socket.h"
#include <cassert>
using namespace anl;

ServerSocket::ServerSocket(ILogger* logger)
{
   this->logger = logger;
}

ServerSocket::~ServerSocket()
{
   stopListening();
}

bool ServerSocket::initialize(int portNumber)
{
   this->portNumber = portNumber;
   this->initialized = true;

   int errorCode = 0;

   //create socket
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

      //bind
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
//
//SocketUPtr ServerSocket::waitAndGetClient()
//{
//   sockaddr_in clientAddrr;
//   int size = sizeof(sockaddr_in);
//   SOCKET newSocket;
//
//
//   newSocket = accept(this->serverSocketHandler, reinterpret_cast<sockaddr*>(&clientAddrr), &size);
//
//   //if something went wrong
//   if(newSocket == INVALID_SOCKET)
//   {
//      return nullptr;
//   }
//   return SocketUPtr(new Socket(this->logger, newSocket, clientAddrr));
//}

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

   //create task to listening
   this->worker = new ClientsListeningTask(this);
   this->listeningThread = std::thread([&]
      {
         this->worker->run();
      });

   return true;
}


void ServerSocket::stopListening()
{
   assert(this->worker && "Start listening, before close it!");
   //clear flags
   this->listening = false;
   this->initialized = false;

   //close socket and listening task
   closesocket(this->serverSocketHandler);
   this->worker->stop();
   this->listeningThread.join();
   delete this->worker;
   this->worker = nullptr;
}

void ServerSocket::pauseListening()
{
   if(nullptr != this->worker)
   {
      this->worker->pause();
   }
}

void ServerSocket::resumeListening()
{
   if(nullptr != this->worker)
   {
      this->worker->resume();
   }
}

void ServerSocket::registerClientConnectedHandler(const ClientConnectedHandler& handler)
{
   this->clientConnectionHandler = handler;
}

void ServerSocket::ClientsListeningTask::run()
{
   listen(this->socket->serverSocketHandler, 0);
   sockaddr_in client;
   int size = sizeof(sockaddr_in);
   while(true)
   {
      this->waitIfPaused();


      SOCKET newSocket;
      newSocket = accept(this->socket->serverSocketHandler, reinterpret_cast<sockaddr*>(&client), &size);

      //if stopListening method was executed, break the loop
      if(true == this->stopRequested())
      {
         break;
      }

      //if something went wrong
      if(newSocket == INVALID_SOCKET)
      {
         this->socket->logger->error("Failed to accepted new client: " + std::to_string(WSAGetLastError()));
         break;
      }

      //if everything is okey, execute callback function
      this->socket->clientConnectionHandler(SocketUPtr(new Socket(this->socket->logger, newSocket, client)));
   }
}
