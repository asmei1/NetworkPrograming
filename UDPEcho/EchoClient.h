#pragma once

#include <QMainWindow>
#include "ui_EchoClient.h"
#include "AsmNetLib.hpp"

class EchoClient : public QMainWindow
{
   Q_OBJECT;

public:
   EchoClient(QWidget* parent = nullptr);
   ~EchoClient();


private:

   //class Listener : public anl::StoppableTask
   //{
   //public:
   //   Listener(anl::UDPSocketSPtr udpSocket);
   //   void run() override;

   //private:
   //   anl::UDPSocketSPtr udpSocket;
   //} listener;
   //std::thread listenerThread;

   anl::UDPSocketSPtr udpSocket;
   Ui::EchoClass ui;
   anl::ILogger* logger;
};
