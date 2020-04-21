#pragma once
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#include "Types.h"
#include "ILogger.hpp"
#include "AsmNetwork.h"
#include "MulticastSocket.h"
#include "UDPSocket.h"
#include "TCPSocket.h"
#include "TCPServerSocket.h"
#include "ResumableTask.hpp"
#include "InetAddress.h"

#include "Exceptions/BindException.h"
#include "Exceptions/TCPServerSocketCreateException.h"
#include "Exceptions/TimeoutException.h"
#include "Exceptions/DatagramSizeOutOfRangeException.h"
