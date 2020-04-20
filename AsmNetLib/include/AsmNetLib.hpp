#pragma once
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#include "Types.h"
#include "ILogger.hpp"
#include "AsmNetwork.h"
#include "UDPSocket.h"
#include "TCPSocket.h"
#include "TCPServerSocket.h"
#include "InetAddress.h"
#include "DatagramSizeOutOfRangeException.h"
#include "TimeoutException.h"
#include "ResumableTask.hpp"
