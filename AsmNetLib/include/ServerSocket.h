#pragma once
#include <winsock2.h>
#include "Types.h"
#include <thread>
#include <atomic>
#include "StoppableTask.hpp"

namespace anl
{
   class ServerSocket
   {
      friend class AsmNetwork;
      ServerSocket(ILogger* logger);
   public:
      ~ServerSocket();

      bool initialize(int portNumber);
      bool isReadyForListening() const;
      SocketUPtr waitAndGetClient();
      bool startListening();
      void stopListening();


      void registerClientConnectedHandler(const ClientConnectedHandler& handler);
   private:
      
      class ClientsListeningTask : public StoppableTask
      {
      public:
         ClientsListeningTask(ServerSocket* serverSocket) : socket(serverSocket){}
         void run() override;
      private:
         ServerSocket* socket;
      };
      ClientsListeningTask* worker;
      std::thread listeningThread;

      std::atomic_bool listening = false;
      ClientConnectedHandler clientConnectionHandler;

      bool initialized = false;
      int portNumber;
      SOCKET serverSocketHandler = SOCKET_ERROR;

      ILogger* logger;
   };
}

