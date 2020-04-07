#pragma once
#include "AsmNetLib.hpp"
#include <QObject>
#include "Helper.hpp"


class Client : public QObject
{
   Q_OBJECT;
public:
   Client(anl::SocketUPtr socket)
   {
      this->socket = std::move(socket);
      this->thread = std::thread{ &Client::run, this };
   }
   ~Client()
   {
      if(this->socket->isConnected())
      {
         closeConnection();
      }
   }

   void closeConnection()
   {
      this->socket->closeSocket();
      this->thread.join();
   }

   void run()
   {
      while(this->socket && this->socket->isConnected())
      {
         const auto& recvData = this->socket->recvData();

         if(recvData)
         {
            emit this->logMessage(("Received from: "
               + anl::socketAddr2String(this->socket->getRawSettings())
               + ", size = " + std::to_string(recvData->size())
               + ": " + std::string{ recvData->begin(), recvData->end() }).c_str());

            if(false == this->socket->sendData(*recvData))
            {
               break;
            }
         }
         else
         {
            emit this->logMessage(("Client disconnected " + anl::socketAddr2String(this->socket->getRawSettings())).c_str());
            emit this->disconnected(this);
            break;
         }
      }
   }
signals:
   void logMessage(const QString& msg);
   void disconnected(Client* clientToRemove);

private:
   std::thread thread;
   anl::SocketUPtr socket;
};

