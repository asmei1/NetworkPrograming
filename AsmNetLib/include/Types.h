#pragma once
#include <memory.h>
#include <functional>

namespace anl
{
   class ILogger;
   class Socket;
   class ServerSocket;

   using Data = std::vector<char>;
   using SocketUPtr = std::unique_ptr<Socket>;
   using ServerSocketUPtr = std::unique_ptr<ServerSocket>;
   using ClientConnectedHandler = std::function<void(SocketUPtr)>;
}