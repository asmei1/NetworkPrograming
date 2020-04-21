#pragma once
#include <memory.h>
#include <functional>

namespace anl
{
   class ILogger;
   class UDPSocket;
   class TCPSocket;
   class MulticastSocket;
   class TCPServerSocket;

   using Data = std::vector<char>;

   using MulticastSocketUPtr = std::unique_ptr<MulticastSocket>;

   using UDPSocketUPtr = std::unique_ptr<UDPSocket>;
   using UDPSocketSPtr = std::shared_ptr<UDPSocket>;

   using TCPSocketUPtr = std::unique_ptr<TCPSocket>;
   using TCPSocketSPtr = std::shared_ptr<TCPSocket>;

   using TCPServerSocketSPtr = std::shared_ptr<TCPServerSocket>;
   using TCPServerSocketUPtr = std::unique_ptr<TCPServerSocket>;

   using ClientConnectedHandler = std::function<void(TCPSocketUPtr)>;

   static inline auto MAX_DATAGRAM_SIZE = 534;
}