#pragma once
#include <string>

class User {

 private:

  std::string ip_;
  std::string user_name_;
  std::string user_password_;
  int quantity_of_connections_;
  bool is_online_;
  int file_descriptor;

 public:

  User(std::string ip, std::string user_name, std::string user_password): ip_(ip),
    user_name_(user_name), user_password_(user_password), is_online_(1) {}

  void OneMoreConnection();
  int GetQuantityOfConnections();
  std::string GetName();
  std::string GetPassword();
  bool IsOnline();
  void SetOnline();
  void SetOffline();
};

void User::OneMoreConnection() {
  quantity_of_connections_++;
}

int User::GetQuantityOfConnections() {
  return quantity_of_connections_;
}

std::string User::GetName() {
  return user_name_;
}

std::string User::GetPassword() {
  return user_password_;
}

bool User::IsOnline() {
  return is_online_;
}

void User::SetOnline() {
  is_online_ = 1;
}

void User::SetOffline() {
  is_online_ = 0;
}

