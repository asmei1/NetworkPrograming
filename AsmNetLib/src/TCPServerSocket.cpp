#include "TCPServerSocket.h"
#include "ILogger.hpp"
#include "TCPSocket.h"
#include <cassert>
using namespace anl;

TCPServerSocket::TCPServerSocket(ILogger* logger)
{
   this->logger = logger;
}

TCPServerSocket::~TCPServerSocket()
{
   stopListening();
}

bool TCPServerSocket::initialize(int portNumber)
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

bool TCPServerSocket::isReadyForListening() const
{
   return this->initialized;
}
//
//TCPSocketUPtr TCPServerSocket::waitAndGetClient()
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
//   return TCPSocketUPtr(new TCPSocket(this->logger, newSocket, clientAddrr));
//}

bool TCPServerSocket::startListening()
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


void TCPServerSocket::stopListening()
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

void TCPServerSocket::pauseListening()
{
   if(nullptr != this->worker)
   {
      this->worker->pause();
   }
}

void TCPServerSocket::resumeListening()
{
   if(nullptr != this->worker)
   {
      this->worker->resume();
   }
}

void TCPServerSocket::registerClientConnectedHandler(const ClientConnectedHandler& handler)
{
   this->clientConnectionHandler = handler;
}

void TCPServerSocket::ClientsListeningTask::run()
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
      this->socket->clientConnectionHandler(TCPSocketUPtr(new TCPSocket(this->socket->logger, newSocket, client)));
   }
}
