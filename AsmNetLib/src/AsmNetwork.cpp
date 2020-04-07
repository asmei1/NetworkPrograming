#include "AsmNetwork.h"
#include "ILogger.hpp"
#include "Socket.h"
#include "ServerSocket.h"

using namespace anl;

bool AsmNetwork::initialize(ILogger* loggerInstance)
{
   AsmNetwork::logger = loggerInstance;

	logger->trace("Initializing Winsock...");
	if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		logger->error("Failed. Error Code : " + std::to_string(WSAGetLastError()));
		return false;
	}
	logger->trace("Initialize");

	return true;
}

int AsmNetwork::cleanup()
{
	return WSACleanup();
}

ILogger* AsmNetwork::getLogger()
{
   return AsmNetwork::logger;
}

SocketUPtr AsmNetwork::createSocket()
{
	return SocketUPtr(new Socket(logger));
}

ServerSocketUPtr AsmNetwork::createServerSocket()
{
	return ServerSocketUPtr(new ServerSocket(logger));
}

