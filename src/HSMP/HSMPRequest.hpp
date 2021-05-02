#pragma once

#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <memory>
#include <vector>

namespace HSMP {

enum RequestType {
  kLoginRequest,
  kListaRequest,
  kMessageRequest,
  kBroadcastRequest,
  kUploadFileRequest,
  kFile_ANRequest,
  kExitRequest
};

struct ClientRequest {
  char accion;

  ClientRequest(char action, RequestType type) : accion(action), _type(type) {}

  virtual ~ClientRequest() {};

  RequestType type() const {
    return _type;
  };

  virtual void PrintStructure() const {};
  virtual char* ParseToCharBuffer() const {
    return nullptr;
  };

 private:
  RequestType _type;
};

struct LoginRequest : ClientRequest {
  short int tam_user;
  short int tam_passwd;
  std::string user;
  std::string passwd;

  LoginRequest() : ClientRequest('l', kLoginRequest) {}
  ~LoginRequest() {}

  void PrintStructure() const override;
  char* ParseToCharBuffer() const override;
};

struct ListaRequest : ClientRequest {
  ListaRequest() : ClientRequest('i', kListaRequest) {}
  ~ListaRequest() {}

  void PrintStructure() const override;
  char* ParseToCharBuffer() const override;
};

struct MessageRequest : ClientRequest {
  short int tam_msg;
  short int tam_destinatario;
  std::string msg;
  std::string destinatario;

  MessageRequest() : ClientRequest('m', kMessageRequest) {}
  ~MessageRequest() {}

  void PrintStructure() const override;
  char* ParseToCharBuffer() const override;
};

struct BroadcastRequest : ClientRequest {
  short int tam_msg;
  std::string msg;

  BroadcastRequest() : ClientRequest('b', kBroadcastRequest) {}
  ~BroadcastRequest() {}

  void PrintStructure() const override;
  char* ParseToCharBuffer() const override;
};

struct UploadFileRequest : ClientRequest {
  short int tam_file_name;
  short int tam_file_data;
  short int tam_destinatario;
  std::string file_name;
  char* file_data;
  std::string destinatario;

  UploadFileRequest() : ClientRequest('u', kUploadFileRequest) {}
  ~UploadFileRequest() {}

  void PrintStructure() const override;

  char* ParseToCharBuffer() const override;
};

struct File_ANRequest : ClientRequest {
  short int tam_remitente;
  std::string remitente;

  File_ANRequest() : ClientRequest('f', kFile_ANRequest) {}
  ~File_ANRequest() {}

  void PrintStructure() const override;
  char* ParseToCharBuffer() const override;
};

struct ExitRequest : ClientRequest {
  ExitRequest() : ClientRequest('x', kExitRequest) {}
  ~ExitRequest() {}

  void PrintStructure() const override;
  char* ParseToCharBuffer() const override;
};

std::shared_ptr<ClientRequest> ProcessRequest(int connectionFD);

}
