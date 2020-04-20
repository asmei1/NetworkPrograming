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

}

EchoServer::~EchoServer()
{
   if(this->serverSocket)
   {
      this->serverSocket->closeSocket();
      this->listener->stop();
      this->listenerThread.join();
      this->listener = nullptr;
   }

   anl::AsmNetwork::cleanup();
}

void EchoServer::on_pushButton_start_clicked()
{

   bool ok;
   const auto& portNumberQStr = this->ui.lineEdit_portNumber->text();
   const auto& portNumber = portNumberQStr.toUInt(&ok);

   if(true == ok)
   {
      try
      {
         this->serverSocket = anl::AsmNetwork::createUDPSocket(portNumber);
         this->listener = std::make_unique<ClientListener>(this->logger, this->serverSocket);

         this->listenerThread = std::thread(&anl::StoppableTask::run, this->listener.get());
         this->logger->info("Server started.");
      }
      catch(int e)
      {
         this->logger->error("There was an error during initialize socket.\nError code: " + std::to_string(portNumber));
      }
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
   this->serverSocket->closeSocket();
   this->listener->stop();
   this->listenerThread.join();
   this->listener = nullptr;
   this->serverSocket = nullptr;
   this->logger->info("Server stopped");


   //GUI
   this->ui.pushButton_stop->setEnabled(false);
   this->ui.pushButton_start->setEnabled(true);
}
