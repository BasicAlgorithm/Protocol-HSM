#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <exception>
#include <stdlib.h>
#include <vector>
#include <string.h>

struct BroadcastResponse {};

class User {
 public:
  std::string ip;
  std::string user_name;
  std::string user_password;
  User(std::string ip,std::string user_name,std::string user_password): ip(ip), user_name(user_name), user_password(user_password){}
};

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
  virtual void ParseToCharBuffer() {};


 private:
  ResponseType _type;
};

struct LoginResponse : ServerResponse {
  std::string ok;

  LoginResponse() : ServerResponse('L', kLoginResponse) {}
  ~LoginResponse() {}

  void PrintStructure() const {
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

  void PrintStructure() const {
    std::cout << "ListaResponse:\n";
    std::cout << "\tnumber of users: " << this->num_users << "\n";
    for(int i = 0; i < num_users; ++i) {
      std::cout << "\tUser #" << i << ": " << this->user_names[i] << "\n";
    }
  }
};

struct MessageResponse : ServerResponse {
  short int tam_msg;
  short int tam_remitente;
  std::string msg;
  std::string remitente;

  MessageResponse() : ServerResponse('M', kMessageResponse) {}
  ~MessageResponse() {}

  void PrintStructure() const {
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
  ~BroadcastResponse(){}

  void PrintStructure() const {
    std::cout << "BroadcastResponse:\n";
    std::cout << "\tmensaje tam: " << this->tam_msg << "\n";
    std::cout << "\tmensaje: " << this->msg << "\n";
    std::cout << "\tremitente tam: " << this->tam_remitente << "\n";
    std::cout << "\tremitente: " << this->remitente << std::endl;
  }

  void ParseToCharBuffer() override {
    // first part of header
    std::string to_server ("B");
    if (this->msg.length() <= 99) {
      to_server+="0";
      if (this->msg.length() <= 9) {
        to_server+="0";
      }
    }
    // second part of header
    to_server += std::to_string(this->msg.length());

    // third part of header
    if (this->remitente.length() <= 9) {
        to_server+="0";
    }
    to_server += std::to_string(this->remitente.length());
    
    // quarter part, body
    to_server += msg + remitente;

    // preparing to send
    char buffer[1003];
    std::size_t length = to_server.copy(buffer,to_server.length(),0);
    buffer[length]='\0';

    // uncomment to know what we are sending
    std::cout << "to send: " << buffer << std::endl; 

  }
};

struct UploadFileResponse : ServerResponse {
  short int tam_file_name;
  short int tam_file_data;
  short int tam_remitente;
  std::string file_name;
  std::string remitente;
  std::string file_data; // castear according to imagen/video/etc

  UploadFileResponse() : ServerResponse('U', kUploadFileResponse) {}
  ~UploadFileResponse(){}

  void PrintStructure() const {
    std::cout << "UploadFileResponse:\n";
    std::cout << "\tfile name tam: " << this->tam_file_name << "\n";
    std::cout << "\tfile: " << this-> file_name << "\n";
    std::cout << "\tfile tam: " << this-> tam_file_data << "\n";
    std::cout << "\tfile_data: " << this-> file_data << "\n";
    std::cout << "\tremitente tam: " << this-> tam_remitente << "\n";
    std::cout << "\tremitente: " << this->remitente << std::endl;
  }

  void ParseToCharBuffer() override {
    // first part of header
    std::string to_server ("U");

    // second part of header
    if (this->file_name.length() <= 99) {
      to_server+="0";
      if (this->file_name.length() <= 9) {
        to_server+="0";
      }
    }
    to_server += std::to_string(this->file_name.length());

    // third part of header
    to_server += std::to_string(this->file_data.length());
    to_server.insert(to_server.begin()+4,14-to_server.length(),'0');

    // quarter part of header
    if (this->remitente.length() <= 9) {
        to_server+="0";
    }
    to_server += std::to_string(this->remitente.length());

    // adding body
    to_server += file_name + file_data + remitente;
    char buffer[1003]; // it should be 16 + 999 + (10 times 9) + 99
    std::size_t length = to_server.copy(buffer,to_server.length(),0);
    buffer[length]='\0';

    // uncomment to know what we are sending
    std::cout << "to send: " << buffer << std::endl; 
  }

};

struct File_ANResponse : ServerResponse {
  short int tam_user_name;
  std::string user_name;

  File_ANResponse() : ServerResponse('F', kFile_ANResponse) {}
  ~File_ANResponse() {}

  void PrintStructure() const {
    std::cout << "File_ANResponse:\n";
    std::cout << "\ttam_user_name: " << this->tam_user_name << "\n";
    std::cout << "\tuser_name: " << this->user_name << std::endl;
  }
};

struct ExitResponse : ServerResponse {
  ExitResponse() : ServerResponse('X', kExitResponse) {}
  ~ExitResponse() {}

  void PrintStructure() const {
    std::cout << "ExitResponse" << std::endl;
  }
};

struct ErrorResponse : ServerResponse {
  std::string message;

  ErrorResponse() : ServerResponse('E', kErrorResponse) {}
  ~ErrorResponse() {}

  void PrintStructure() const {
    std::cout << "ErrorResponse:\n";
    std::cout << "\tmessage: " << this->message << std::endl;
  }
};

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
  virtual char *ParseToCharBuffer() {return nullptr;};
  virtual void ProcessThisRequest(std::shared_ptr<std::list<User>> users) {};

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
  short int tam_msg;
  std::string msg;

  BroadcastRequest() : ClientRequest('b', kBroadcastRequest) {}
  ~BroadcastRequest() {}

  void PrintStructure() const {
    std::cout << "BroadcastRequest:\n";
    std::cout << "\tmensaje tam: " << this->tam_msg << "\n";
    std::cout << "\tmensaje: " << this->msg << std::endl;
  }

  char *ParseToCharBuffer() override {
    // first part of header
    std::string to_server ("b");
    if (this->msg.length() <= 99) {
      to_server+="0";
      if (this->msg.length() <= 9) {
        to_server+="0";
      }
    }
    // second part of header + body
    to_server += std::to_string(this->msg.length()) + this->msg;

    char *buffer= new char[to_server.length() + 1];
    std::size_t length = to_server.copy(buffer,to_server.length(),0);
    buffer[length]='\0';

    // Uncomment to know what we are sending
    //std::cout << "\tBroadcast sending to server: " << buffer << std::endl; 

    return buffer;
  }
  
  void ProcessThisRequest(std::shared_ptr<std::list<User>> users) override {
    auto response = std::make_shared<BroadcastResponse>();
    std::list<User>::iterator it=users->begin();
    std::string remitente = it->user_name;
    ++it;
    for (; it != users->end(); ++it) {
      std::cout << "user to send: " << it->user_name << std::endl;
      response->msg = this->msg;
      response->remitente = remitente;
      response->ParseToCharBuffer();
    }
  }

};

struct UploadFileRequest : ClientRequest {
  short int tam_file_name;
  short int tam_file_data;
  short int tam_destinatario;
  std::string file_name;
  std::string file_data; // castear segun es imagen/video/etc
  std::string destinatario;

  UploadFileRequest() : ClientRequest('u', kUploadFileRequest) {}
  ~UploadFileRequest() {}

  void PrintStructure() const {
    std::cout << "UploadFileRequest:\n";
    std::cout << "\tfile nombre tam: " << this->tam_file_name << "\n";
    std::cout << "\tfile nombre: " << this->file_name << "\n";
    std::cout << "\tfile tam: " << this->tam_file_data << "\n";
    std::cout << "\tdestinatario tam: " << this->file_data << "\n";
    std::cout << "\ftam_destinatario: " << this->tam_destinatario << "\n";
    std::cout << "\tdestinatario : " << this->destinatario << std::endl;
  }
  
  char *ParseToCharBuffer() override {
    // first part of header
    std::string to_server ("u");

    // second part of header
    if (this->file_name.length() <= 99) {
      to_server+="0";
      if (this->file_name.length() <= 9) {
        to_server+="0";
      }
    }
    to_server += std::to_string(this->file_name.length());

    // third part of header
    to_server += std::to_string(this->file_data.length());
    to_server.insert(to_server.begin()+4,14-to_server.length(),'0');

    // quarter part of header
    if (this->destinatario.length() <= 9) {
        to_server+="0";
    }
    to_server += std::to_string(this->destinatario.length());

    // adding body
    to_server += file_name + file_data + destinatario;
    char *buffer= new char[to_server.length() + 1];
    std::size_t length = to_server.copy(buffer,to_server.length(),0);
    buffer[length]='\0';

    // uncomment to know what we are sending
    //std::cout << "\tUploadFileRequest sending to server: " << buffer << std::endl; 

    return buffer;
  }
  
  void ProcessThisRequest(std::shared_ptr<std::list<User>> users) override {
    std::string who_do_request = "miguel"; // this have to be dinamically
    auto response = std::make_shared<UploadFileResponse>();
    response->file_name = this->file_name;
    response->remitente = who_do_request;
    response->file_data = this->file_data;
    response->ParseToCharBuffer();
  }
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

std::shared_ptr<ClientRequest> ProcessRequest (const void *request) {
  std::string buffer((char*)request);
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

    case 'b': {
      auto breq = std::make_shared<BroadcastRequest>();

      breq->tam_msg = atoi(buffer.substr(1, 3).c_str());

      breq->msg = buffer.substr(4, breq->tam_msg);
      return breq;
    }

    case 'u': {
      auto ureq = std::make_shared<UploadFileRequest>();

      ureq->tam_file_name = atoi(buffer.substr(1, 3).c_str());
      ureq->tam_file_data = atoi(buffer.substr(4, 10).c_str());
      ureq->tam_destinatario = atoi(buffer.substr(14, 2).c_str());

      ureq->file_name = buffer.substr(16, ureq->tam_file_name);
      ureq->file_data = buffer.substr(16+ ureq->tam_file_name,ureq->tam_file_data); // depende el archivo
      ureq->destinatario = buffer.substr(16 + ureq->tam_file_name + ureq->tam_file_data, ureq->tam_destinatario);
      return ureq;
    }

    case 'f': {
      auto freq = std::make_shared<File_ANRequest>();
      
      freq->tam_remitente = atoi(buffer.substr(1, 2).c_str());
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

std::shared_ptr<ServerResponse> ProcessResponse (const void *response) {
  std::string buffer((char*)response);
  char action = buffer[0];

  switch (action) {
    case 'L': {
      auto Lres = std::make_shared<LoginResponse>();
      Lres->ok = buffer.substr(1, 2);
      return Lres;
    }

    case 'I': {
      auto Ires = std::make_shared<ListaResponse>();

      Ires->num_users = atoi(buffer.substr(1, 2).c_str());
      for (int i = 0; i < Ires->num_users; ++i) {
        Ires->tam_user_names.push_back(atoi(buffer.substr(2 * i + 3, 2).c_str()));
      }
      short int current_position = 2 * Ires->num_users + 3;
      for (int i = 0; i < Ires->num_users; ++i) {
        Ires->user_names.push_back(buffer.substr(current_position, Ires->tam_user_names[i]));
        current_position += Ires->tam_user_names[i];
      }
      return Ires;
    }

    case 'M': {
      auto Mres = std::make_shared<MessageResponse>();

      Mres->tam_msg = atoi(buffer.substr(1, 3).c_str());
      Mres->tam_remitente = atoi(buffer.substr(4, 2).c_str());
      
      Mres->msg = buffer.substr(6, Mres->tam_msg);
      Mres->remitente = buffer.substr(6 + Mres->tam_msg, Mres->tam_remitente);
      
      return Mres;
    }

    case 'B': {
      auto Mres = std::make_shared<BroadcastResponse>();

      Mres->tam_msg = atoi(buffer.substr(1, 3).c_str());
      Mres->tam_remitente = atoi(buffer.substr(4, 2).c_str());
      
      Mres->msg = buffer.substr(6, Mres->tam_msg);
      Mres->remitente = buffer.substr(6 + Mres->tam_msg, Mres->tam_remitente);
      
      return Mres;
    }

    case 'U': {
      auto Mres = std::make_shared<UploadFileResponse>();

      Mres->tam_file_name = atoi(buffer.substr(1, 3).c_str());
      Mres->tam_file_data = atoi(buffer.substr(4, 10).c_str());
      Mres->tam_remitente = atoi(buffer.substr(14, 2).c_str());

      Mres->file_name = buffer.substr(16, Mres->tam_file_name);
      Mres->file_data = buffer.substr(16+ Mres->tam_file_name,Mres->tam_file_data); // depende el archivo
      Mres->remitente = buffer.substr(16 + Mres->tam_file_name + Mres->tam_file_data, Mres->tam_remitente);
      return Mres;
    }

    case 'F': {
      auto Fres = std::make_shared<File_ANResponse>();
      Fres->tam_user_name = atoi(buffer.substr(1, 2).c_str());
      Fres->user_name = buffer.substr(3, Fres->tam_user_name);
      return Fres;
    }

    case 'X': {
      auto Xres = std::make_shared<ExitResponse>();
      return Xres;
    }

    case 'E': {
      auto Eres = std::make_shared<ErrorResponse>();
      Eres->message = buffer.substr(1);
      return Eres;
    }

    default:
      return nullptr;
  }
}

std::shared_ptr<ClientRequest> CreateRequest(){

  // TODO
  // infinite while for create diferente request without execute ./hsmp client

  char accion;
  printf("What do you want to do? [l] [i] [m] [b] [u] [x]\n");
  std::cin >> accion;
  std::cin.ignore();

switch (accion) {
  case 'b':
  {
    auto breq = std::make_shared<HSMP::BroadcastRequest>();
    std::cout << "Creando Broadcast Request" << '\n';

    char msgs[999];
    std::cout << "what is the message to send: ";
    std::cin.getline(msgs,999,'\n');
    breq->msg = std::string(msgs);

    return breq;
  }
  case 'u':
  {
    auto ureq = std::make_shared<HSMP::UploadFileRequest>();
    std::cout << "Creando UploadFile Request" << '\n';

    char buffer_name_file[999];
    std::cout << "what is the file'name: ";
    std::cin.getline(buffer_name_file,999,'\n');
    ureq->file_name = std::string(buffer_name_file);

    char buffer_data_file[999]; // it should be 10 times 9
    std::cout << "what is the file'data: ";
    std::cin.getline(buffer_data_file,999,'\n');
    ureq->file_data = std::string(buffer_data_file);

    char buffer_name_receptor[99];
    std::cout << "what is the receptor'name: ";
    std::cin.getline(buffer_name_receptor,999,'\n');
    ureq->destinatario = std::string(buffer_name_receptor);

    return ureq; 
  }
  default:
  {
    std::cout << "wrong input" << std::endl;
    return nullptr;
  }
  }
}

}
