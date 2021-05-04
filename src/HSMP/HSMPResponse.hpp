#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <exception>
#include <stdlib.h>
#include <vector>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

namespace HSMP {

enum ResponseType {
  kLoginResponse,
  kListaResponse,
  kMessageResponse,
  kBroadcastResponse,
  kUploadFileResponse,
  kFile_ANResponse,
  kExitResponse,
  kErrorResponse
};

struct ServerResponse {
  char accion;

  ServerResponse(char action, ResponseType type) : accion(action), _type(type) {}

  virtual ~ServerResponse() {};

  ResponseType type() const {
    return _type;
  };

  virtual void PrintStructure() const {};
  virtual char* ParseToCharBuffer() const {
    return nullptr;
  };

 private:
  ResponseType _type;
};

struct LoginResponse : ServerResponse {
  std::string ok;

  LoginResponse() : ServerResponse('L', kLoginResponse) {}
  ~LoginResponse() {}

  void PrintStructure() const override;
  char *ParseToCharBuffer() const override;

};

struct ListaResponse : ServerResponse {
  short int num_users;
  std::vector<short int> tam_user_names;
  std::vector<std::string> user_names;

  ListaResponse() : ServerResponse('I', kListaResponse) {}
  ~ListaResponse() {}

  void PrintStructure() const override;
  char *ParseToCharBuffer() const override;

};

struct MessageResponse : ServerResponse {
  short int tam_msg;
  short int tam_remitente;
  std::string msg;
  std::string remitente;

  MessageResponse() : ServerResponse('M', kMessageResponse) {}
  ~MessageResponse() {}

  void PrintStructure() const override;
  char *ParseToCharBuffer() const override;

};

struct BroadcastResponse : ServerResponse {
  short int tam_msg;
  short int tam_remitente;
  std::string msg;
  std::string remitente;

  BroadcastResponse(): ServerResponse('B', kBroadcastResponse) {}
  ~BroadcastResponse() {}

  void PrintStructure() const override;
  char *ParseToCharBuffer() const override;
};

struct UploadFileResponse : ServerResponse {
  short int tam_file_name;
  short int tam_file_data;
  short int tam_remitente;
  std::string file_name;
  std::string remitente;
  char* file_data;

  UploadFileResponse() : ServerResponse('U', kUploadFileResponse) {}
  ~UploadFileResponse() {}

  void PrintStructure() const override;
  char *ParseToCharBuffer() const override;
};

struct File_ANResponse : ServerResponse {
  short int tam_user_name;
  std::string user_name;

  File_ANResponse() : ServerResponse('F', kFile_ANResponse) {}
  ~File_ANResponse() {}

  void PrintStructure() const override;
};

struct ExitResponse : ServerResponse {

  ExitResponse() : ServerResponse('X', kExitResponse) {}
  ~ExitResponse() {}

  void PrintStructure() const override;
  char *ParseToCharBuffer() const override;

};

struct ErrorResponse : ServerResponse {
  std::string message;

  ErrorResponse() : ServerResponse('E', kErrorResponse) {}
  ~ErrorResponse() {}

  void PrintStructure() const override;
  char *ParseToCharBuffer() const override;

};

std::shared_ptr<ServerResponse> ProcessResponse(int connectionFD);

}
