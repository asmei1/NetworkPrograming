#include "Helper.hpp"
#include <winsock2.h>

bool anl::replace(std::string& str, const std::string& from, const std::string& to)
{
   size_t start_pos = str.find(from);
   if(start_pos == std::string::npos)
      return false;
   str.replace(start_pos, from.length(), to);
   return true;
}

bool anl::chkNumber(const std::string& str)
{
   return false == str.empty() && (str.find_first_not_of("[0123456789]") == std::string::npos);
}

std::vector<std::string> anl::split(const std::string& str, char delim)
{
   auto i = 0;
   std::vector<std::string> list;
   auto pos = str.find(delim);
   while(pos != std::string::npos) 
   {
      list.push_back(str.substr(i, pos - i));
      i = ++pos;
      pos = str.find(delim, pos);
   }
   list.push_back(str.substr(i, str.length()));
   return list;
}

bool anl::validateIP(const std::string& ip)
{
   // split the std::string into tokens
   std::vector<std::string> slist = split(ip, '.');
   // if token size is not equal to four
   if(slist.size() != 4)
   {
      return false;
   }

   for(const auto& str : slist) 
   {
      // check that std::string is number, positive, and range
      if(false == chkNumber(str) || std::stoi(str) < 0 || std::stoi(str) > 255)
      {
         return false;
      }
   }
   return true;
}

std::string anl::hostNameToIP(const std::string& hostName)
{
   struct hostent* he = gethostbyname(hostName.c_str());
   std::string ip;

   if(nullptr == he)
   {
      //gethostbyname failed
      throw WSAGetLastError();
   }
   struct in_addr** addr_list;
   //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
   addr_list = (struct in_addr**)he;

   if(addr_list[0] != nullptr)
   {
      //Return the first one;
      ip = inet_ntoa(*addr_list[0]);
   }
   return ip;
}

std::optional<std::string> anl::parseAddress(const std::string& hostName)
{
   std::string ip = hostName;

   if(false == validateIP(hostName))
   {
      if(hostName == "localhost")
      {
         ip = "127.0.0.1";
      }
      else
      {
         try
         {
            ip = hostNameToIP(hostName);
         }
         catch(...)
         {
            return std::nullopt;
         }
      }
   }
   return ip;
}
