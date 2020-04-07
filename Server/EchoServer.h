#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_EchoServer.h"
#include "AsmNetLib.hpp"


class EchoServer : public QMainWindow
{
   Q_OBJECT;

public:
   EchoServer(QWidget* parent = nullptr);
   ~EchoServer();

   void handleClient();

private slots:
   void on_pushButton_start_clicked();
   void on_pushButton_stop_clicked();

private:
   std::atomic_bool listening = true;
   anl::ILogger* logger;
   anl::SocketUPtr clientSocket;
   anl::ServerSocketUPtr serverSocket{nullptr};
   Ui::ServerClass ui;
};
