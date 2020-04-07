#include "EchoServer.h"
#include "QLogger.h"
#include "Helper.hpp"
#include <QtConcurrent/QtConcurrent>
#include <execution>

EchoServer::EchoServer(QWidget* parent)
   : QMainWindow(parent)
{
   ui.setupUi(this);
   this->ui.pushButton_stop->setEnabled(false);

   auto tempLogger = new QLogger("[%TIME%] [%LOG_LEVEL%]: ");
   connect(tempLogger, &QLogger::logSig, this->ui.plainTextEdit_output, &QPlainTextEdit::appendHtml);
   this->logger = tempLogger;

   anl::AsmNetwork::initialize(this->logger);
   this->serverSocket = anl::AsmNetwork::createServerSocket();

   this->serverSocket->registerClientConnectedHandler([this](anl::SocketUPtr socket)
      {
         this->handleNewClient(std::forward<anl::SocketUPtr>(socket));
      });

}

EchoServer::~EchoServer()
{
   anl::AsmNetwork::cleanup();
}

void EchoServer::handleNewClient(anl::SocketUPtr newClient)
{
   this->logger->info("New client connected " + anl::socketAddr2String(newClient->getRawSettings()));
   auto client = new Client(std::move(newClient));
   connect(client, &Client::logMessage, this, [this](const QString& msg)
      {
         this->logger->info(msg.toStdString());
      });
   connect(client, &Client::disconnected, this, &EchoServer::clientDisconnected);


   std::lock_guard<std::mutex>(this->clientsMutex);
   this->clients.emplace_back(client);
}

void EchoServer::on_pushButton_start_clicked()
{
   bool ok;
   const auto& portNumberQStr = this->ui.lineEdit_portNumber->text();
   const auto& portNumber = portNumberQStr.toUInt(&ok);

   if(true == ok)
   {
      if(false == this->serverSocket->isReadyForListening())
      {
         if(0 != this->serverSocket->initialize(portNumber))
         {
            return;
         }
      }

      this->serverSocket->startListening();
   }

   this->ui.pushButton_start->setEnabled(false);
   this->ui.pushButton_stop->setEnabled(true);
}

void EchoServer::on_pushButton_stop_clicked()
{
   this->logger->info("Server stopped");

   this->serverSocket->stopListening();
   this->clients.clear();
 //  std::for_each(std::execution::par, this->clients.begin(), this->clients.end(), std::mem_fn(&Client::closeConnection));
   /*[this]
   (Client* client)
      {
         client->closeConnection();
         delete client;
      });*/

   //std::for_each(std::execution::par, this->clientSockets.begin(), this->clientSockets.end(), [this]
   //(auto& par)
   //   {
   //      this->logger->info("Client disconnected " + anl::socketAddr2String(par.first->getRawSettings()));
   //      par.first->closeSocket();
   //      par.second.join();
   //   });
   //this->clientSockets.clear();

   this->ui.pushButton_stop->setEnabled(false);
   this->ui.pushButton_start->setEnabled(true);
}

void EchoServer::clientDisconnected(Client* clientToRemove)
{
   std::lock_guard<std::mutex>(this->clientsMutex);
   auto it = std::find_if(this->clients.begin(), this->clients.end(), [clientToRemove](const auto& ptr)
   {
         return ptr.get() == clientToRemove;
   });

   if(it != this->clients.end())
   {
      this->clients.erase(it);
   }
}
