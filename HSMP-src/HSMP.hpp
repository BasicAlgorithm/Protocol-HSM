#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <exception>
#include <stdlib.h>

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

  void PrintStructure() const {
    std::cout << "LoginRequest:\n";
    std::cout << "\ttam_user: " << this->tam_user << "\n";
    std::cout << "\ttam_passwd: " << this->tam_passwd << "\n";
    std::cout << "\tuser: " << this->user << "\n";
    std::cout << "\tpasswd: " << this->passwd << std::endl;
  }
};

struct ListaRequest : ClientRequest {
  ListaRequest() : ClientRequest('i', kListaRequest) {}
  ~ListaRequest() {}

  void PrintStructure() const {
    std::cout << "ListaRequest" << std::endl;
  }
};

struct MessageRequest : ClientRequest {
  short int tam_msg;
  short int tam_destinatario;
  std::string msg;
  std::string destinatario;

  MessageRequest() : ClientRequest('m', kMessageRequest) {}
  ~MessageRequest() {}

  void PrintStructure() const {
    std::cout << "MessageRequest:\n";
    std::cout << "\ttam_msg: " << this->tam_msg << "\n";
    std::cout << "\ttam_destinatario: " << this->tam_destinatario << "\n";
    std::cout << "\tmsg: " << this->msg << "\n";
    std::cout << "\tdestinatario: " << this->destinatario << std::endl;
  }
};

struct BroadcastRequest : ClientRequest {
  ~BroadcastRequest() {}
  void PrintStructure() const {}
};

struct UploadFileRequest : ClientRequest {
  ~UploadFileRequest() {}
  void PrintStructure() const {}
};

struct File_ANRequest : ClientRequest {
  short int tam_remitente;
  std::string remitente;

  File_ANRequest() : ClientRequest('f', kFile_ANRequest) {}
  ~File_ANRequest() {}

  void PrintStructure() const {
    std::cout << "File_ANRequest:\n";
    std::cout << "\ttam_remitente: " << this->tam_remitente << "\n";
    std::cout << "\tremitente: " << this->remitente << std::endl;
  }
};

struct ExitRequest : ClientRequest {
  ExitRequest() : ClientRequest('x', kExitRequest) {}
  ~ExitRequest() {}

  void PrintStructure() const {
    std::cout << "ExitRequest" << std::endl;
  }
};

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

 private:
  ResponseType _type;
};

struct LoginResponse : ServerResponse {
  char ok[2];

  LoginResponse() : ServerResponse('L', kLoginResponse) {}
  ~LoginResponse() {}

  void PrintStructure() const {}
};

struct ListaResponse : ServerResponse {
  char num_users[2];
  char tam_user_name[2];
  char* user_name;

  ListaResponse() : ServerResponse('I', kListaResponse) {}
  ~ListaResponse() {}

  void PrintStructure() const {}
};

struct MessageResponse : ServerResponse {
  char tam_msg[3];
  char tam_remitente[2];
  char* msg;
  char* remitente;

  MessageResponse() : ServerResponse('M', kMessageResponse) {}
  ~MessageResponse() {}

  void PrintStructure() const {}
};

struct BroadcastResponse : ServerResponse {};

struct UploadFileResponse : ServerResponse {};

struct File_ANResponse : ServerResponse {
  char tam_usuario[2];
  char* usuario;

  File_ANResponse() : ServerResponse('F', kFile_ANResponse) {}
  ~File_ANResponse() {}

  void PrintStructure() const {}
};

struct ExitResponse : ServerResponse { // X
};

struct ErrorResponse : ServerResponse {
  char message[20];

  ErrorResponse() : ServerResponse('E', kErrorResponse) {}
  ~ErrorResponse() {}

  void PrintStructure() const {}
};


std::shared_ptr<ClientRequest> ProcessRequest(std::string buffer) {
  char action = buffer[0];

  switch (action) {
    case 'l': {
      auto lreq = std::make_shared<LoginRequest>();

      lreq->tam_user = atoi(buffer.substr(1, 2).c_str());
      lreq->tam_passwd = atoi(buffer.substr(3, 2).c_str());

      lreq->user = buffer.substr(5, lreq->tam_user);
      lreq->passwd = buffer.substr(5 + lreq->tam_user, lreq->tam_passwd);

      return lreq;
    }

    case 'i': {
      auto ireq = std::make_shared<ListaRequest>();
      return ireq;
    }

    case 'm': {
      auto mreq = std::make_shared<MessageRequest>();

      mreq->tam_msg = atoi(buffer.substr(1, 3).c_str());
      mreq->tam_destinatario = atoi(buffer.substr(4, 2).c_str());

      mreq->msg = buffer.substr(6, mreq->tam_msg);
      mreq->destinatario = buffer.substr(6 + mreq->tam_msg, mreq->tam_destinatario);
      return mreq;
    }

    case 'f': {
      auto freq = std::make_shared<File_ANRequest>();
      freq->tam_remitente = atoi(buffer.substr(1,2).c_str());
      freq->remitente = buffer.substr(3, freq->tam_remitente);
      return freq;
    }

    case 'x': {
      auto xreq = std::make_shared<ExitRequest>();
      return xreq;
    }

    default:
      return nullptr;
  }
}

// static void ProcessResponse(char* buffer); //read -->
// static void SendRequest(std::string request);// --> write
// static void SendResponse(std::string response);
//
// static void ProcessLoginRequest();
// static void ProcessListaRequest();
// static void ProcessMessageRequest();
// static void ProcessBroadcastRequest();
// static void ProcessUploadFileRequest();
// static void ProcessFile_ANRequest();
// static void ProcessExitRequest();
//
// static void ProcessLoginResponse();
// static void ProcessListaResponse();
// static void ProcessMessageResponse();
// static void ProcessBroadcastResponse();
// static void ProcessUploadFileResponse();
// static void ProcessFile_ANResponse();
// static void ProcessExitResponse();
// static void ProcessErrorResponse();
}