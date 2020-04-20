#pragma once
#include <winsock2.h>
#include "Types.h"
#include <thread>
#include <atomic>
#include "ResumableTask.hpp"
#include <string>

namespace anl
{
   
   class TCPServerSocket
   {
      friend class AsmNetwork;
      TCPServerSocket(ILogger* logger);
   public:
      ~TCPServerSocket();

      bool initialize(int portNumber);
      bool isReadyForListening() const;
      bool startListening();
      void stopListening();

      void pauseListening();
      void resumeListening();

      void registerClientConnectedHandler(const ClientConnectedHandler& handler);
   private:
      
      class ClientsListeningTask : public ResumableTask
      {
      public:
         ClientsListeningTask(TCPServerSocket* serverSocket) : socket(serverSocket){}
         void run() override;
      private:
         TCPServerSocket* socket;
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

