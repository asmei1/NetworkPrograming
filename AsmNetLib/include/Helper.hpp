#pragma once
#include <string>
#include <vector>
#include <optional>

namespace anl
{
   bool replace(std::string& str, const std::string& from, const std::string& to);
   bool chkNumber(const std::string& str);
   // Function to split std::string str using given delimiter
   std::vector<std::string> split(const std::string& str, char delim);
   // Function to validate an IP address
   bool validateIP(const std::string& ip);

   std::string hostNameToIP(const std::string& hostName);
   std::optional<std::string> parseAddress(const std::string& hostName);
   
}
