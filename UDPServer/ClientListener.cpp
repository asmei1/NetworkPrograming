#include "ClientListener.h"
#include <QtConcurrent/QtConcurrent>
#include <algorithm>

ClientListener::ClientListener(anl::ILogger* logger, anl::UDPSocketSPtr socket)
{
   this->socket = socket;
   this->logger = logger;
}

void ClientListener::run()
{
   while(true)
   {
      if(stopRequested() == true)
      {
         break;
      }

      anl::Data data;
      try
      {
         const auto& address = this->socket->recvData(data);
         //text is send, so i can assume, that end of string is \0 and rest of data is removable trash 
         data.erase(std::remove(data.begin(), data.end(), '\0'),data.end());

         auto msg = std::string{ data.begin(), data.end() };
         this->logger->info(QString("Received message from: %1. Message: %2")
            .arg(anl::socketAddr2String(address).c_str())
            .arg(msg.c_str())
            .toStdString());
         
         QtConcurrent::run([=]
            {
               std::lock_guard(this->mutex);
               try
               {
                  this->socket->sendData(data, address);
               }
               catch(int err)
               {
                  this->logger->error(QString("Cannot send message to %1.\nError code: %2")
                     .arg(anl::socketAddr2String(address).c_str())
                     .arg(err)
                     .toStdString());
               }
            });
      }
      catch(int err)
      {
         if(err == 10022)
         {
            this->logger->error("Port is already blocked.\nError code: " + std::to_string(err));
            break;
         }
         else if(err != 10004)
         {
            this->logger->error("Cannot receive message.\nError code: " + std::to_string(err));
         }
      }
   }
}
