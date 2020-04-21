#include "EchoClient.h"
#include "BasicLogger.hpp"
#include "QLogger.h"
#include <QtConcurrent/QtConcurrent>

EchoClient::EchoClient(QWidget* parent) : QMainWindow(parent)
{
   this->ui.setupUi(this);

   auto tempLogger = new QLogger("[%TIME%] [%LOG_LEVEL%]: ");
   connect(tempLogger, &QLogger::logSig, this->ui.plainTextEdit_output, &QPlainTextEdit::appendHtml);
   this->logger = tempLogger;

   anl::AsmNetwork::initialize(this->logger);

   this->udpSocket = anl::AsmNetwork::createUDPSocket();

   connect(this->ui.pushButton_send, &QPushButton::clicked, this, [this]
      {
         bool ok;
         const auto& ipAddress = this->ui.lineEdit_hostName->text().toStdString();
         const auto& portNumberQStr = this->ui.lineEdit_portNumber->text();
         const auto& portNumber = portNumberQStr.toUInt(&ok);

         if(false == ipAddress.empty() && true == ok)
         {
            const auto& message = this->ui.textEdit_message->toPlainText().toStdString();
            try
            {
               anl::InetAddress addr( ipAddress, portNumber );
               anl::Data dataToSend{ message.begin(), message.end() };
               int msgSize = dataToSend.size();
               //dataToSend.resize(anl::MAX_DATAGRAM_SIZE);

               this->udpSocket->sendData(dataToSend, addr);
               this->logger->info("Send: " + message);
               std::thread([this, addr, msgSize]
                  {
                     anl::Data recvData(msgSize);
                     try
                     {
                        this->udpSocket->recvData(recvData, addr, 1000000);
                        this->logger->info("Received: " + std::string{ recvData.begin(), recvData.end() });
                     }
                     catch(const anl::DatagramSizeOutOfRangeException& e)
                     {
                        this->logger->error("Cannot send a message with length greater than " + std::to_string(anl::MAX_DATAGRAM_SIZE));
                     }
                     catch(const anl::TimeoutException& e)
                     {
                        this->logger->error("Timeout. Cannot get a return message from server.");
                     }
                     catch(int err)
                     {
                        this->logger->error("Server didn't respond.\nError code: " + std::to_string(err));
                     }
                  }).detach();   
            }
            catch(int err)
            {
               this->logger->error("Cannot send a message. Error code: " + std::to_string(err));
            }
         }
         else
         {
            this->logger->error("Destination ip address is empty or port number is invalid.");
         }
      });
}


EchoClient::~EchoClient()
{
   /*if(this->udpSocket)
   {
      this->udpSocket->closeSocket();
   }
   this->listener.stop();
   this->listenerThread.join();*/

   anl::AsmNetwork::cleanup();
}
//
//void EchoClient::Listener::run()
//{
//}

