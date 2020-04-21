#pragma once

#include <QMainWindow>
#include "ui_MulticastWindow.h"
#include "AsmNetLib.hpp"

class MulticastWindow : public QMainWindow
{
   Q_OBJECT;

public:
   MulticastWindow(QWidget* parent = nullptr);
   ~MulticastWindow();

private slots:
   void on_pushButton_send_clicked();
   void on_pushButton_startListening_clicked();
   void on_pushButton_stopListening_clicked();

private:

   class BroadcastListener : public anl::StoppableTask
   {
      anl::UDPSocketUPtr socket;
      anl::ILogger* logger;
   public:
      BroadcastListener(anl::ILogger* logger, int port)
      {
         this->logger = logger;
         this->socket = anl::AsmNetwork::createBroadcastSocket(port);
      }
      void run() override;

   };

   class MulticastListener : public anl::StoppableTask
   {
      anl::MulticastSocketUPtr socket;
      anl::ILogger* logger;
   public:
      MulticastListener(anl::ILogger* logger, const anl::InetAddress& addr)
      {
         this->logger = logger;
         this->socket = anl::AsmNetwork::createMulticastSocket();
         this->socket->initilizeRecv(addr);
      }
      void run() override;

   };
   std::unique_ptr<BroadcastListener> broadcastListener;
   std::unique_ptr<MulticastListener> multicastListener;
   std::thread listenerThread;



   Ui::MulticastAppClass ui;
   anl::ILogger* logger;
};
