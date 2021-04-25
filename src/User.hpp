#pragma once
#include <string>

class User {
 public:
  std::string ip;
  std::string user_name;
  std::string user_password;
  User(std::string ip, std::string user_name, std::string user_password): ip(ip),
    user_name(user_name), user_password(user_password) {}
};

