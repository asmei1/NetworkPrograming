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

   //logger initialize
   auto tempLogger = new QLogger("[%TIME%] [%LOG_LEVEL%]: ");
   connect(tempLogger, &QLogger::logSig, this->ui.plainTextEdit_output, &QPlainTextEdit::appendHtml);
   this->logger = tempLogger;

   //Network library initialize
   anl::AsmNetwork::initialize(this->logger);
   this->serverSocket = anl::AsmNetwork::createServerSocket();

   //add function handler for new clients 
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
   std::lock_guard<std::mutex>(this->clientsMutex);

   //if max clients limit reach, close new client
   if(this->clients.size() >= this->maxClientNumber)
   {
      newClient->closeSocket();
   }
   else
   {

      this->logger->info("New client connected " + anl::socketAddr2String(newClient->getRawSettings()));

      auto client = new Client(std::move(newClient));
      connect(client, &Client::logMessage, this, [this](const QString& msg)
         {
            this->logger->info(msg.toStdString());
         });
      connect(client, &Client::disconnected, this, &EchoServer::clientDisconnected);

      this->clients.emplace_back(client);


      this->logger->info("Clients[" + std::to_string(this->clients.size()) + "/" + std::to_string(this->maxClientNumber) + "]");
   }
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
   else
   {
      this->logger->error("Wrong port number " + portNumberQStr.toStdString());
   }

   this->ui.pushButton_start->setEnabled(false);
   this->ui.pushButton_stop->setEnabled(true);
}

void EchoServer::on_pushButton_stop_clicked()
{
   this->logger->info("Server stopped");

   this->serverSocket->stopListening();
   this->clients.clear();

   //GUI
   this->ui.pushButton_stop->setEnabled(false);
   this->ui.pushButton_start->setEnabled(true);
}

void EchoServer::clientDisconnected(Client* clientToRemove)
{
   std::lock_guard<std::mutex>(this->clientsMutex);
   //find
   auto it = std::find_if(this->clients.begin(), this->clients.end(), [clientToRemove](const auto& ptr)
   {
         return ptr.get() == clientToRemove;
   });

   //if client exist, remove it from list
   if(it != this->clients.end())
   {
      this->clients.erase(it);
   }

   this->logger->info("Clients[" + std::to_string(this->clients.size()) + ":" + std::to_string(this->maxClientNumber) + "]");
}
