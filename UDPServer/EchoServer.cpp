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

}

EchoServer::~EchoServer()
{
   anl::AsmNetwork::cleanup();
}

void EchoServer::on_pushButton_start_clicked()
{
   //bool ok;
   //const auto& portNumberQStr = this->ui.lineEdit_portNumber->text();
   //const auto& portNumber = portNumberQStr.toUInt(&ok);

   //if(true == ok)
   //{
   //   if(false == this->serverSocket->isReadyForListening())
   //   {
   //      if(0 != this->serverSocket->initialize(portNumber))
   //      {
   //         return;
   //      }
   //   }

   //   this->serverSocket->startListening();
   //}
   //else
   //{
   //   this->logger->error("Wrong port number " + portNumberQStr.toStdString());
   //}

   this->ui.pushButton_start->setEnabled(false);
   this->ui.pushButton_stop->setEnabled(true);
}

void EchoServer::on_pushButton_stop_clicked()
{
   this->logger->info("Server stopped");

   
   //GUI
   this->ui.pushButton_stop->setEnabled(false);
   this->ui.pushButton_start->setEnabled(true);
}
