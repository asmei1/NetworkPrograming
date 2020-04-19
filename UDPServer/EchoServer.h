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

private slots:
   void on_pushButton_start_clicked();
   void on_pushButton_stop_clicked();

private:
   anl::ILogger* logger;
   
   Ui::ServerClass ui;

   const int maxClientNumber = 3;
};
