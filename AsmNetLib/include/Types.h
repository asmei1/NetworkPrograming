#pragma once
#include <memory.h>
#include <functional>

namespace anl
{
   class ILogger;
   class UDPSocket;
   class TCPSocket;
   class TCPServerSocket;

   using Data = std::vector<char>;
   using UDPSocketUPtr = std::unique_ptr<UDPSocket>;
   using UDPSocketSPtr = std::shared_ptr<UDPSocket>;
   using TCPSocketUPtr = std::unique_ptr<TCPSocket>;
   using TCPServerSocketUPtr = std::unique_ptr<TCPServerSocket>;
   using ClientConnectedHandler = std::function<void(TCPSocketUPtr)>;

   static inline auto MAX_DATAGRAM_SIZE = 534;
}