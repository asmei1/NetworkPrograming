#include "AsmNetwork.h"
#include "ILogger.hpp"
#include "TCPSocket.h"
#include "TCPServerSocket.h"
#include "UDPSocket.h"
#include "MulticastSocket.h"

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

MulticastSocketUPtr AsmNetwork::createMulticastSocket()
{
	return MulticastSocketUPtr(new MulticastSocket(logger));
}

TCPSocketUPtr AsmNetwork::createTCPSocket()
{
	return TCPSocketUPtr(new TCPSocket(logger));
}

UDPSocketUPtr AsmNetwork::createServerUDPSocket(uint16_t portNumber)
{
	return UDPSocketUPtr(new UDPSocket(logger, portNumber));
}

UDPSocketUPtr AsmNetwork::createUDPSocket()
{
	return UDPSocketUPtr(new UDPSocket(logger));
}

UDPSocketUPtr AsmNetwork::createBroadcastSocket(uint16_t portNumber)
{
	auto socket = new UDPSocket(logger, portNumber, true);

	return UDPSocketUPtr(socket);
}

TCPServerSocketUPtr AsmNetwork::createTCPServerSocket()
{
	return TCPServerSocketUPtr(new TCPServerSocket(logger));
}

