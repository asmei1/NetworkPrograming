#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_EchoClient.h"
#include "AsmNetLib.hpp"

class EchoClient : public QMainWindow
{
   Q_OBJECT;

public:
   EchoClient(QWidget* parent = nullptr);
   ~EchoClient();


private:
   Ui::EchoClass ui;
   anl::SocketUPtr socket{nullptr};
   anl::ILogger* logger;
};
