#pragma once
#include "Helper.hpp"

namespace anl
{
   class InetAddress
   {
   public:
      InetAddress(const std::string& hostName, const int& portNumber)
      {
         const auto& ipOpt = parseAddress(hostName);

         this->addrr.sin_addr.s_addr = inet_addr(ipOpt->c_str());
         this->addrr.sin_family = AF_INET;
         this->addrr.sin_port = htons(portNumber);
      }

      InetAddress(const sockaddr_in& addrr)
      {
         this->addrr = addrr;
      }

      sockaddr_in getRawSettings() const
      {
         return this->addrr;
      }

      sockaddr* toSockAddr() const
      {
         return (sockaddr*)&addrr;
      }

      int getStructSize() const
      {
         return sizeof(addrr);
      }

   private:
      sockaddr_in addrr;

   };
}
