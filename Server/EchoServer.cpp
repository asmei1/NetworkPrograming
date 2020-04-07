#include "EchoServer.h"
#include "QLogger.h"
#include "Helper.hpp"
#include <QtConcurrent/QtConcurrent>

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

}

EchoServer::~EchoServer()
{
   anl::AsmNetwork::cleanup();
}

void EchoServer::handleClient()
{
   while(true)
   {
      if(false == this->listening)
      {
         return;
      }
      this->serverSocket->startListening();


      this->clientSocket = std::move(this->serverSocket->waitAndGetClient());

      this->serverSocket->stopListening();


      if(nullptr != this->clientSocket.get())
      {
         this->logger->info("New client connected " + anl::socketAddr2String(this->clientSocket->getRawSettings()));

         while(this->clientSocket)
         {
            const auto& recvData = this->clientSocket->recvData();
            if(false == this->listening)
            {
               return;
            }

            if(recvData)
            {
               this->logger->info("Received from: "
                  + anl::socketAddr2String(this->clientSocket->getRawSettings())
                  + ", size = " + std::to_string(recvData->size())
                  + ": " + std::string{ recvData->begin(), recvData->end() });

               if(false == this->clientSocket->sendData(*recvData))
               {
                  break;
               }
            }
            else
            {
               this->logger->info("Client disconnected " + anl::socketAddr2String(this->clientSocket->getRawSettings()));

               break;
            }
         }
      }
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
      this->listening = true;

      //to no block GUI
      QtConcurrent::run([this]
         {
            handleClient();
         });
   }

   this->ui.pushButton_start->setEnabled(false);
   this->ui.pushButton_stop->setEnabled(true);
}

void EchoServer::on_pushButton_stop_clicked()
{
   this->logger->info("Server stopped");

   this->listening = false;
   this->serverSocket->stopListening();

   if(this->clientSocket)
   {
      this->logger->info("Client disconnected " + anl::socketAddr2String(this->clientSocket->getRawSettings()));
      this->clientSocket->closeSocket();
   }
   this->clientSocket = nullptr;

   this->ui.pushButton_stop->setEnabled(false);
   this->ui.pushButton_start->setEnabled(true);
}