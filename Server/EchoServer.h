#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_EchoServer.h"
#include "AsmNetLib.hpp"
#include "Client.h"


class EchoServer : public QMainWindow
{
   Q_OBJECT;

public:
   EchoServer(QWidget* parent = nullptr);
   ~EchoServer();

   void handleNewClient(anl::SocketUPtr newClient);

private slots:
   void on_pushButton_start_clicked();
   void on_pushButton_stop_clicked();

   void clientDisconnected(Client* clientToRemove);
private:
   anl::ILogger* logger;
   
   std::mutex clientsMutex;
   std::vector<std::unique_ptr<Client>> clients;
   anl::ServerSocketUPtr serverSocket{nullptr};
   Ui::ServerClass ui;
};
