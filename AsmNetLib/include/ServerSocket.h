#pragma once
#include <winsock2.h>

namespace anl
{
   class ServerSocket
   {
      ServerSocket();
   public:
      ~ServerSocket();

      bool initialize(int portNumber);

   private:
      int portNumber;
      SOCKET serverSocketHandler;
   };
}

