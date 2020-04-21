#include "MulticastWindow.h"
#include "BasicLogger.hpp"
#include "QLogger.h"
#include <QtConcurrent/QtConcurrent>

MulticastWindow::MulticastWindow(QWidget* parent) : QMainWindow(parent)
{
   this->ui.setupUi(this);
   this->ui.pushButton_startListening->setEnabled(true);
   this->ui.pushButton_stopListening->setEnabled(false);


   auto tempLogger = new QLogger("[%TIME%] [%LOG_LEVEL%]: ");
   connect(tempLogger, &QLogger::logSig, this->ui.plainTextEdit_output, &QPlainTextEdit::appendHtml);
   this->logger = tempLogger;

   anl::AsmNetwork::initialize(this->logger);

}


MulticastWindow::~MulticastWindow()
{
   if(this->broadcastListener)
   {
      this->broadcastListener->stop();
      this->broadcastListener = nullptr;
   }
   if(this->multicastListener)
   {
      this->multicastListener->stop();
      this->multicastListener = nullptr;
   }
   if(true == this->listenerThread.joinable())
   {
      this->listenerThread.join();
   }

   anl::AsmNetwork::cleanup();
}

void MulticastWindow::on_pushButton_send_clicked()
{
   bool ok;
   const auto& ipAddress = this->ui.lineEdit_hostName->text().toStdString();
   const auto& portNumberQStr = this->ui.lineEdit_portNumber->text();
   const auto& portNumber = portNumberQStr.toUInt(&ok);

   if(false == ipAddress.empty() && true == ok)
   {
      const auto& message = this->ui.textEdit_message->toPlainText().toStdString();
      anl::Data dataToSend{ message.begin(), message.end() };
      try
      {
         if(this->ui.checkBox_broadcastEnabled->checkState() == Qt::Checked)
         {
            anl::AsmNetwork::createBroadcastSocket(portNumber)->sendData(dataToSend, anl::InetAddress::broadcastAddress(portNumber));
         }
         else
         {
            anl::AsmNetwork::createMulticastSocket()->sendData(dataToSend, anl::InetAddress(ipAddress, portNumber));
         }
      }
      catch(int err)
      {
         this->logger->error("Error code: " + std::to_string(err));
      }
   }
}

void MulticastWindow::on_pushButton_startListening_clicked()
{
   bool ok;
   const auto& ipAddress = this->ui.lineEdit_hostName->text().toStdString();
   const auto& portNumberQStr = this->ui.lineEdit_portNumber->text();
   const auto& portNumber = portNumberQStr.toUInt(&ok);

   if(false == ipAddress.empty() && true == ok)
   {
      try
      {
         if(this->ui.checkBox_broadcastEnabled->checkState() == Qt::Checked)
         {
            this->broadcastListener = std::make_unique<BroadcastListener>(this->logger, portNumber);
            this->listenerThread = std::thread(&anl::StoppableTask::run, this->broadcastListener.get());
         }
         else
         {
            this->multicastListener = std::make_unique<MulticastListener>(this->logger, anl::InetAddress(ipAddress, portNumber));
            this->listenerThread = std::thread(&anl::StoppableTask::run, this->multicastListener.get());
         }
         this->ui.pushButton_startListening->setEnabled(false);
         this->ui.pushButton_stopListening->setEnabled(true);
         this->ui.checkBox_broadcastEnabled->setEnabled(false);
      }
      catch(int err)
      {
         this->logger->error("Server failed. Error code: " + std::to_string(err));
      }

   }
}

void MulticastWindow::on_pushButton_stopListening_clicked()
{
   if(this->broadcastListener)
   {
      this->broadcastListener->stop();
      this->broadcastListener = nullptr;
   }
   if(this->multicastListener)
   {
      this->multicastListener->stop();
      this->multicastListener = nullptr;
   }
   if(true == this->listenerThread.joinable())
   {
      this->listenerThread.join();
   }


   this->ui.pushButton_startListening->setEnabled(true);
   this->ui.pushButton_stopListening->setEnabled(false);
   this->ui.checkBox_broadcastEnabled->setEnabled(true);
}

void MulticastWindow::BroadcastListener::run()
{
   this->logger->info("Broadcast listener started");
   while(true)
   {
      if(true == this->stopRequested())
      {
         break;
      }

      anl::Data recvData;
      try
      {
         const auto& address = this->socket->recvData(recvData);
         this->logger->info("Received: " + std::string{ recvData.begin(), recvData.end() } +"\nfrom " + anl::socketAddr2String(address));
      }
      catch(int err)
      {
         if(err != 10004)
         {
            this->logger->error("Error during listening for incoming packages. Check selected port!\nError code: " + std::to_string(err));
         }
         else if(err != 10004)
         {
            this->logger->error("Error code: " + std::to_string(err));
         }
         break;
      }
   }
   this->logger->info("Broadcast listener stopped");
}

void MulticastWindow::MulticastListener::run()
{
   this->logger->info("Multicast listener started");
   while(true)
   {
      if(true == this->stopRequested())
      {
         break;
      }

      anl::Data recvData;
      try
      {
         this->socket->recvData(recvData);
         this->logger->info("Received: " + std::string{ recvData.begin(), recvData.end() } /*+"\nfrom " + anl::socketAddr2String(address)*/);
      }
      catch(int err)
      {
         if(err != 10004)
         {
            this->logger->error("Error during listening for incoming packages. Check selected port!\nError code: " + std::to_string(err));
         }
         else if(err != 10004)
         {
            this->logger->error("Error code: " + std::to_string(err));
         }
         break;
      }
   }
   this->logger->info("Multicast listener stopped");
}

