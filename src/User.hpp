#pragma once
#include <string>

class User {

 private:

  std::string ip_;
  std::string user_name_;
  std::string user_password_;
  int quantity_of_connections_;
  bool is_online_;
  int file_descriptor_;

 public:

  User(std::string ip, std::string user_name, std::string user_password);
  
  User();

  void OneMoreConnection();
  int GetQuantityOfConnections();
  int GetFileDescriptor();
  std::string GetName();
  std::string GetPassword();
  void SetIP(std::string new_ip);
  bool IsOnline();
  void SetOnline();
  void SetOffline();
  void SetFileDescriptor(int new_fd);
  std::string GetIp();

};

// =============================================================================

User::User (std::string ip, std::string user_name, std::string user_password) :
  ip_(ip), user_name_(user_name), user_password_(user_password), is_online_(0) {};

User::User (){}

void User::OneMoreConnection() {
  quantity_of_connections_++;
}

int User::GetQuantityOfConnections() {
  return quantity_of_connections_;
}

std::string User::GetName() {
  return user_name_;
}

void User::SetIP(std::string new_ip) {
  this->ip_ = new_ip;
}

std::string User::GetPassword() {
  return user_password_;
}

bool User::IsOnline() {
  return is_online_;
}

void User::SetFileDescriptor(int new_fd) {
  this->file_descriptor_ = new_fd;
}

void User::SetOnline() {
  this->is_online_ = 1;
}

void User::SetOffline() {
  this->is_online_ = 0;
}

std::string User::GetIp() {
  return ip_;
}

int User::GetFileDescriptor() {
  return file_descriptor_;
}

