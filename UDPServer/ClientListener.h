#pragma once
#include "AsmNetLib.hpp"
#include <QObject>

class ClientListener : public anl::StoppableTask
{
public:
   ClientListener(anl::ILogger* logger, anl::UDPSocketSPtr socket);
   void run() override;

private:
   std::mutex mutex;
   anl::UDPSocketSPtr socket;
   anl::ILogger* logger;
};

