#pragma once
#include <string>

class User {

 private:

  std::string ip_;
  std::string user_name_;
  std::string user_password_;
  int quantity_of_connections;

 public:

  User(std::string ip, std::string user_name, std::string user_password): ip_(ip),
    user_name_(user_name), user_password_(user_password) {}

  void OneMoreConnection();
  int GetQuantityOfConnections();
  std::string GetName();
  std::string GetPassword();
};

void User::OneMoreConnection() {
  quantity_of_connections++;
}

int User::GetQuantityOfConnections() {
  return quantity_of_connections;
}

std::string User::GetName() {
  return user_name_;
}

std::string User::GetPassword() {
  return user_password_;
}

