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

class User {
 public:
  std::string ip;
  std::string user_name;
  std::string user_password;
  User(std::string ip, std::string user_name, std::string user_password): ip(ip),
    user_name(user_name), user_password(user_password) {}
};

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

struct ClientRequest {
  char accion;

  ClientRequest(char action, RequestType type) : accion(action), _type(type) {}

  virtual ~ClientRequest() {};

  RequestType type() const {
    return _type;
  };

  virtual void PrintStructure() const {};
  virtual char* ParseToCharBuffer() {
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

  void PrintStructure() const override {
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

  void PrintStructure() const override {
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

  void PrintStructure() const override {
    std::cout << "MessageRequest:\n";
    std::cout << "\ttam_msg: " << this->tam_msg << "\n";
    std::cout << "\ttam_destinatario: " << this->tam_destinatario << "\n";
    std::cout << "\tmsg: " << this->msg << "\n";
    std::cout << "\tdestinatario: " << this->destinatario << std::endl;
  }
};

struct BroadcastRequest : ClientRequest {
  short int tam_msg;
  std::string msg;

  BroadcastRequest() : ClientRequest('b', kBroadcastRequest) {}
  ~BroadcastRequest() {}

  void PrintStructure() const override {
    std::cout << "BroadcastRequest:\n";
    std::cout << "\tmensaje tam: " << this->tam_msg << "\n";
    std::cout << "\tmensaje: " << this->msg << std::endl;
  }

  char* ParseToCharBuffer() override {
    std::string parsed_structure("b");

    if (this->msg.length() <= 99) {
      parsed_structure += "0";

      if (this->msg.length() <= 9)
        parsed_structure += "0";
    }

    parsed_structure += std::to_string(this->msg.length()) + this->msg;

    char* buffer= new char[parsed_structure.length() + 1];
    std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(),
                         0);
    buffer[length]='\0';

    //std::cout << "\tBroadcast sending to server: " << buffer << std::endl;

    return buffer;
  }
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

  void PrintStructure() const override {
    std::cout << "UploadFileRequest:\n";
    std::cout << "\tfile nombre tam: " << this->tam_file_name << "\n";
    std::cout << "\tfile nombre: " << this->file_name << "\n";
    std::cout << "\tfile tam: " << this->tam_file_data << "\n";
    std::cout << "\tdestinatario tam: " << this->file_data << "\n";
    std::cout << "\ftam_destinatario: " << this->tam_destinatario << "\n";
    std::cout << "\tdestinatario : " << this->destinatario << std::endl;
  }

  char* ParseToCharBuffer() override {
    std::string parsed_structure("u");

    if (this->file_name.length() <= 99) {
      parsed_structure += "0";

      if (this->file_name.length() <= 9)
        parsed_structure += "0";
    }

    parsed_structure += std::to_string(this->file_name.length());

    parsed_structure += std::to_string(this->tam_file_data);
    parsed_structure.insert(parsed_structure.begin()+4, 14-parsed_structure.length(),
                            '0');

    if (this->destinatario.length() <= 9)
      parsed_structure += "0";

    parsed_structure += std::to_string(this->destinatario.length());

    parsed_structure += file_name + file_data + destinatario;
    char* buffer= new char[parsed_structure.length() + 1];
    std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(), 0);
    buffer[length]='\0';

    // std::cout << "\tUploadFileRequest sending to server: " << buffer << std::endl;

    return buffer;
  }
};

struct File_ANRequest : ClientRequest {
  short int tam_remitente;
  std::string remitente;

  File_ANRequest() : ClientRequest('f', kFile_ANRequest) {}
  ~File_ANRequest() {}

  void PrintStructure() const override {
    std::cout << "File_ANRequest:\n";
    std::cout << "\ttam_remitente: " << this->tam_remitente << "\n";
    std::cout << "\tremitente: " << this->remitente << std::endl;
  }
};

struct ExitRequest : ClientRequest {
  ExitRequest() : ClientRequest('x', kExitRequest) {}
  ~ExitRequest() {}

  void PrintStructure() const override {
    std::cout << "ExitRequest" << std::endl;
  }
};

struct ServerResponse {
  char accion;

  ServerResponse(char action, ResponseType type) : accion(action), _type(type) {}

  virtual ~ServerResponse() {};

  ResponseType type() const {
    return _type;
  };

  virtual void PrintStructure() const {};
  virtual void ParseToCharBuffer() {};

 private:
  ResponseType _type;
};

struct LoginResponse : ServerResponse {
  std::string ok;

  LoginResponse() : ServerResponse('L', kLoginResponse) {}
  ~LoginResponse() {}

  void PrintStructure() const override {
    std::cout << "LoginResponse:\n";
    std::cout << "\tLogin state: " << this->ok << std::endl;
  }
};

struct ListaResponse : ServerResponse {
  short int num_users;
  std::vector<short int> tam_user_names;
  std::vector<std::string> user_names;

  ListaResponse() : ServerResponse('I', kListaResponse) {}
  ~ListaResponse() {}

  void PrintStructure() const override {
    std::cout << "ListaResponse:\n";
    std::cout << "\tnumber of users: " << this->num_users << "\n";

    for (int i = 0; i < num_users; ++i)
      std::cout << "\tUser #" << i << ": " << this->user_names[i] << "\n";
  }
};

struct MessageResponse : ServerResponse {
  short int tam_msg;
  short int tam_remitente;
  std::string msg;
  std::string remitente;

  MessageResponse() : ServerResponse('M', kMessageResponse) {}
  ~MessageResponse() {}

  void PrintStructure() const override {
    std::cout << "MessageResponse:\n";
    std::cout << "\ttam_msg: " << this->tam_msg << "\n";
    std::cout << "\ttam_remitente: " << this->tam_remitente << "\n";
    std::cout << "\tmsg: " << this->msg << "\n";
    std::cout << "\tremitente: " << this->remitente << std::endl;
  }
};

struct BroadcastResponse : ServerResponse {
  short int tam_msg;
  short int tam_remitente;
  std::string msg;
  std::string remitente;

  BroadcastResponse(): ServerResponse('B', kBroadcastResponse) {}
  ~BroadcastResponse() {}

  void PrintStructure() const override {
    std::cout << "BroadcastResponse:\n";
    std::cout << "\tmensaje tam: " << this->tam_msg << "\n";
    std::cout << "\tmensaje: " << this->msg << "\n";
    std::cout << "\tremitente tam: " << this->tam_remitente << "\n";
    std::cout << "\tremitente: " << this->remitente << std::endl;
  }

  void ParseToCharBuffer() override {
    std::string parsed_structure("B");

    if (this->msg.length() <= 99) {
      parsed_structure += "0";

      if (this->msg.length() <= 9)
        parsed_structure += "0";
    }

    parsed_structure += std::to_string(this->msg.length());

    if (this->remitente.length() <= 9)
      parsed_structure += "0";

    parsed_structure += std::to_string(this->remitente.length());

    parsed_structure += msg + remitente;

    char buffer[1003];
    std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(),
                         0);
    buffer[length]='\0';

    std::cout << "to send: " << buffer << std::endl;
  }
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

  void PrintStructure() const override {
    std::cout << "UploadFileResponse:\n";
    std::cout << "\tfile name tam: " << this->tam_file_name << "\n";
    std::cout << "\tfile: " << this-> file_name << "\n";
    std::cout << "\tfile tam: " << this-> tam_file_data << "\n";
    std::cout << "\tfile_data: " << this-> file_data << "\n";
    std::cout << "\tremitente tam: " << this-> tam_remitente << "\n";
    std::cout << "\tremitente: " << this->remitente << std::endl;
  }

  void ParseToCharBuffer() override {
    std::string parsed_structure("U");

    if (this->file_name.length() <= 99) {
      parsed_structure += "0";

      if (this->file_name.length() <= 9)
        parsed_structure += "0";
    }

    parsed_structure += std::to_string(this->file_name.length());

    parsed_structure += std::to_string(this->tam_file_data);
    parsed_structure.insert(parsed_structure.begin()+4,
                            14-parsed_structure.length(), '0');

    if (this->remitente.length() <= 9)
      parsed_structure += "0";

    parsed_structure += std::to_string(this->remitente.length());

    parsed_structure += file_name + file_data + remitente;
    char buffer[1003]; // it should be 16 + 999 + (10 times 9) + 99
    std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(),
                         0);
    buffer[length]='\0';

    std::cout << "to send: " << buffer << std::endl;
  }
};

struct File_ANResponse : ServerResponse {
  short int tam_user_name;
  std::string user_name;

  File_ANResponse() : ServerResponse('F', kFile_ANResponse) {}
  ~File_ANResponse() {}

  void PrintStructure() const override {
    std::cout << "File_ANResponse:\n";
    std::cout << "\ttam_user_name: " << this->tam_user_name << "\n";
    std::cout << "\tuser_name: " << this->user_name << std::endl;
  }
};

struct ExitResponse : ServerResponse {
  ExitResponse() : ServerResponse('X', kExitResponse) {}
  ~ExitResponse() {}

  void PrintStructure() const override {
    std::cout << "ExitResponse" << std::endl;
  }
};

struct ErrorResponse : ServerResponse {
  std::string message;

  ErrorResponse() : ServerResponse('E', kErrorResponse) {}
  ~ErrorResponse() {}

  void PrintStructure() const override {
    std::cout << "ErrorResponse:\n";
    std::cout << "\tmessage: " << this->message << std::endl;
  }
};

std::shared_ptr<ClientRequest> ProcessRequest(int connectionFD) {
  char buffer[1024] = {0};
  read(connectionFD, buffer, 1);
  char action = buffer[0];
  std::string s;

  switch (action) {
    case 'l': {
      auto lreq = std::make_shared<LoginRequest>();

      read(connectionFD, buffer, 4);
      s = buffer;
      lreq->tam_user = stoi(s.substr(0, 2));
      lreq->tam_passwd = stoi(s.substr(2, 2));

      bzero(buffer, 4);
      read(connectionFD, buffer, lreq->tam_user + lreq->tam_passwd);
      s = buffer;
      lreq->user = s.substr(0, lreq->tam_user);
      lreq->passwd = s.substr(lreq->tam_user, lreq->tam_passwd);

      return lreq;
    }

    case 'i': {
      auto ireq = std::make_shared<ListaRequest>();
      return ireq;
    }

    case 'm': {
      auto mreq = std::make_shared<MessageRequest>();

      read(connectionFD, buffer, 5);
      s = buffer;
      mreq->tam_msg = stoi(s.substr(0, 3));
      mreq->tam_destinatario = stoi(s.substr(3, 2));

      bzero(buffer, 5);
      read(connectionFD, buffer, mreq->tam_msg + mreq->tam_destinatario);
      s = buffer;
      mreq->msg = s.substr(0, mreq->tam_msg);
      mreq->destinatario = s.substr(mreq->tam_msg, mreq->tam_destinatario);

      return mreq;
    }

    case 'b': {
      auto breq = std::make_shared<BroadcastRequest>();
      read(connectionFD, buffer, 3);
      breq->tam_msg = atoi(buffer);

      bzero(buffer, 3);
      read(connectionFD, buffer, breq->tam_msg);
      breq->msg = buffer;

      return breq;
    }

    case 'u': {
      auto ureq = std::make_shared<UploadFileRequest>();

      read(connectionFD, buffer, 15);
      s = buffer;
      ureq->tam_file_name = stoi(s.substr(0, 3));
      ureq->tam_file_data = stoi(s.substr(3, 10));
      ureq->tam_destinatario = stoi(s.substr(13, 2));

      bzero(buffer, 15);
      read(connectionFD, buffer, ureq->tam_file_name);
      ureq->file_name = buffer;

      ureq->file_data = new char[ureq->tam_file_data];
      read(connectionFD, ureq->file_data, ureq->tam_file_data);

      bzero(buffer, ureq->tam_file_name);
      read(connectionFD, buffer, ureq->tam_destinatario);
      ureq->destinatario = buffer;

      return ureq;
    }

    case 'f': {
      auto freq = std::make_shared<File_ANRequest>();

      read(connectionFD, buffer, 2);
      freq->tam_remitente = atoi(buffer);

      bzero(buffer, 2);
      read(connectionFD, buffer, freq->tam_remitente);
      freq->remitente = buffer;

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

std::shared_ptr<ServerResponse> ProcessResponse(int connectionFD) {
  char buffer[1024] = {0};
  read(connectionFD, buffer, 1);
  char action = buffer[0];
  std::string s;

  switch (action) {
    case 'L': {
      auto Lres = std::make_shared<LoginResponse>();

      read(connectionFD, buffer, 2);
      Lres->ok = buffer;

      return Lres;
    }

    case 'I': {
      auto Ires = std::make_shared<ListaResponse>();

      read(connectionFD, buffer, 4);
      s = buffer;
      Ires->num_users = stoi(s.substr(0, 2));

      read(connectionFD, buffer, 2*Ires->num_users);
      s = buffer;
      short int total_length_names = 0;

      for (int i = 0; i < Ires->num_users; ++i) {
        int tam_user_name = stoi(s.substr(i*2, 2));
        Ires->tam_user_names.push_back(tam_user_name);
        total_length_names += tam_user_name;
      }

      read(connectionFD, buffer, total_length_names);
      s = buffer;

      for (int i = 0, current_position = 0; i < Ires->num_users; ++i) {
        Ires->user_names.push_back(s.substr(current_position, Ires->tam_user_names[i]));
        current_position += Ires->tam_user_names[i];
      }

      return Ires;
    }

    case 'M': {
      auto Mres = std::make_shared<MessageResponse>();

      read(connectionFD, buffer, 5);
      s = buffer;
      Mres->tam_msg = stoi(s.substr(0, 3));
      Mres->tam_remitente = stoi(s.substr(3, 2));

      read(connectionFD, buffer, Mres->tam_msg + Mres->tam_remitente);
      s = buffer;
      Mres->msg = s.substr(0, Mres->tam_msg);
      Mres->remitente = s.substr(Mres->tam_msg, Mres->tam_remitente);

      return Mres;
    }

    case 'B': {
      auto Bres = std::make_shared<BroadcastResponse>();

      read(connectionFD, buffer, 5);
      s = buffer;
      Bres->tam_msg = stoi(s.substr(0, 3));
      Bres->tam_remitente = stoi(s.substr(3, 2));

      bzero(buffer, 5);
      read(connectionFD, buffer, Bres->tam_msg + Bres->tam_remitente);
      s = buffer;
      Bres->msg = s.substr(0, Bres->tam_msg);
      Bres->remitente = s.substr(Bres->tam_msg, Bres->tam_remitente);

      return Bres;
    }

    case 'U': {
      auto Ures = std::make_shared<UploadFileResponse>();

      read(connectionFD, buffer, 15);
      s = buffer;
      Ures->tam_file_name = stoi(s.substr(0, 3));
      Ures->tam_file_data = stoi(s.substr(3, 10));
      Ures->tam_remitente = stoi(s.substr(13, 2));

      bzero(buffer, 15);
      read(connectionFD, buffer, Ures->tam_file_name);
      Ures->file_name = buffer;

      Ures->file_data = new char[Ures->tam_file_data];
      read(connectionFD, Ures->file_data, Ures->tam_file_data);

      bzero(buffer, Ures->tam_file_name);
      read(connectionFD, buffer, Ures->tam_remitente);
      Ures->remitente = buffer;

      return Ures;
    }

    case 'F': {
      auto Fres = std::make_shared<File_ANResponse>();

      read(connectionFD, buffer, 2);
      Fres->tam_user_name = atoi(buffer);

      bzero(buffer, 2);

      read(connectionFD, buffer, Fres->tam_user_name);
      Fres->user_name = buffer;

      return Fres;
    }

    case 'X': {
      auto Xres = std::make_shared<ExitResponse>();
      return Xres;
    }

    case 'E': {
      auto Eres = std::make_shared<ErrorResponse>();

      read(connectionFD, buffer, 20);
      Eres->message = buffer;

      return Eres;
    }

    default:
      return nullptr;
  }
}

std::shared_ptr<ClientRequest> CreateRequest() {
  // TODO
  // infinite while to create different requests without executing ./hsmp client
  char accion;
  printf("What do you want to do? [l] [i] [m] [b] [u] [x]\n");
  std::cin >> accion;
  std::cin.ignore();

  switch (accion) {
    case 'b': {
      auto breq = std::make_shared<HSMP::BroadcastRequest>();
      std::cout << "Creando Broadcast Request" << '\n';

      char msgs[999];
      std::cout << "what is the message to send: ";
      std::cin.getline(msgs, 999, '\n');
      breq->msg = std::string(msgs);

      return breq;
    }

    case 'u': {
      auto ureq = std::make_shared<HSMP::UploadFileRequest>();
      std::cout << "Creando UploadFile Request" << '\n';

      char buffer_name_file[999];
      std::cout << "what is the file'name: ";
      std::cin.getline(buffer_name_file, 999, '\n');
      ureq->file_name = std::string(buffer_name_file);

      char buffer_data_file[999]; // it should be 10 times 9
      std::cout << "what is the file'data: ";
      std::cin.getline(buffer_data_file, 999, '\n');
      ureq->file_data = new char[strlen(buffer_data_file)];
      strcpy(ureq->file_data, buffer_data_file);

      char buffer_name_receptor[99];
      std::cout << "what is the receptor'name: ";
      std::cin.getline(buffer_name_receptor, 999, '\n');
      ureq->destinatario = std::string(buffer_name_receptor);

      return ureq;
    }

    default: {
      std::cout << "wrong input" << std::endl;
      return nullptr;
    }
  }
}

}
