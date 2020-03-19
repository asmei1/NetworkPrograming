#include "EchoClient.h"
#include "BasicLogger.hpp"
#include "QLogger.h"

EchoClient::EchoClient(QWidget* parent) : QMainWindow(parent)
{
   this->ui.setupUi(this);
   this->ui.pushButton_send->setEnabled(false);

   auto tempLogger = new QLogger("[%TIME%] [%LOG_LEVEL%]: ");
   connect(tempLogger, &QLogger::logSig, this->ui.plainTextEdit_output, &QPlainTextEdit::appendHtml);
   this->logger = tempLogger;

   anl::AsmNetwork::initialize(this->logger);

   connect(this->ui.pushButton_connect, &QPushButton::clicked, this, [this]
      {
         bool ok;
         const auto& ipAddress = this->ui.lineEdit_hostName->text().toStdString();
         const auto& portNumberQStr = this->ui.lineEdit_portNumber->text();
         const auto& portNumber = portNumberQStr.toUInt(&ok);

         if(false == ipAddress.empty() && true == ok)
         {
            this->socket = anl::AsmNetwork::createSocket();
            this->socket->initialize();

            if(true == this->socket->connectTo(ipAddress, portNumber))
            {
               this->ui.pushButton_send->setEnabled(true);
               this->ui.pushButton_connect->setEnabled(false);

               this->logger->info("Connected to server.");
            }
         }
         else
         {
            this->logger->error("Cannot connect to server.\nCheck server address or port number!");
         }
      });

   connect(this->ui.pushButton_send, &QPushButton::clicked, this, [this]
      {
         const auto& message = this->ui.textEdit_message->toPlainText().toStdString();
         if(true == this->socket->sendData({ message.begin(), message.end() }))
         {
            this->logger->info("Sended: " + message);

            const auto& recvData = this->socket->recvData();

            if(recvData)
            {
               this->logger->info("Received: " + std::string{ recvData->begin(), recvData->end() });
            }
            else
            {
               this->logger->error("Server stopped responded.");
            }

            this->ui.textEdit_message->clear();
         }
         else
         {
            this->logger->error("Cannot send a message.");
         }
      });


}


EchoClient::~EchoClient()
{
   anl::AsmNetwork::cleanup();
}