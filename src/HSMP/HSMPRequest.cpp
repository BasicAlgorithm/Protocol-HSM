#include "HSMPRequest.hpp"

namespace HSMP {

void LoginRequest::PrintStructure() const {
  std::cout << "LoginRequest:\n";
  std::cout << "\ttam_user: " << this->tam_user << "\n";
  std::cout << "\ttam_passwd: " << this->tam_passwd << "\n";
  std::cout << "\tuser: " << this->user << "\n";
  std::cout << "\tpasswd: " << this->passwd << std::endl;
}

void ListaRequest::PrintStructure() const {
  std::cout << "ListaRequest" << std::endl;
}

void MessageRequest::PrintStructure() const {
  std::cout << "MessageRequest:\n";
  std::cout << "\ttam_msg: " << this->tam_msg << "\n";
  std::cout << "\ttam_destinatario: " << this->tam_destinatario << "\n";
  std::cout << "\tmsg: " << this->msg << "\n";
  std::cout << "\tdestinatario: " << this->destinatario << std::endl;
}

void BroadcastRequest::PrintStructure() const {
  std::cout << "BroadcastRequest:\n";
  std::cout << "\tmensaje tam: " << this->tam_msg << "\n";
  std::cout << "\tmensaje: " << this->msg << std::endl;
}

char* BroadcastRequest::ParseToCharBuffer() const {
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

void UploadFileRequest::PrintStructure() const {
  std::cout << "UploadFileRequest:\n";
  std::cout << "\tfile nombre tam: " << this->tam_file_name << "\n";
  std::cout << "\tfile nombre: " << this->file_name << "\n";
  std::cout << "\tfile tam: " << this->tam_file_data << "\n";
  std::cout << "\tdestinatario tam: " << this->file_data << "\n";
  std::cout << "\ftam_destinatario: " << this->tam_destinatario << "\n";
  std::cout << "\tdestinatario : " << this->destinatario << std::endl;
}

char* UploadFileRequest::ParseToCharBuffer() const {
  std::string parsed_structure("u");

  if (this->file_name.length() <= 99) {
    parsed_structure += "0";

    if (this->file_name.length() <= 9)
      parsed_structure += "0";
  }

  parsed_structure += std::to_string(this->file_name.length());

  parsed_structure += std::to_string(this->tam_file_data);
  parsed_structure.insert(parsed_structure.begin()+4,
                          14-parsed_structure.length(),
                          '0');

  if (this->destinatario.length() <= 9)
    parsed_structure += "0";

  parsed_structure += std::to_string(this->destinatario.length());

  parsed_structure += file_name + file_data + destinatario;
  char* buffer= new char[parsed_structure.length() + 1];
  std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(),
                       0);
  buffer[length]='\0';

  // std::cout << "\tUploadFileRequest sending to server: " << buffer << std::endl;

  return buffer;
}

void File_ANRequest::PrintStructure() const {
  std::cout << "File_ANRequest:\n";
  std::cout << "\ttam_remitente: " << this->tam_remitente << "\n";
  std::cout << "\tremitente: " << this->remitente << std::endl;
}

void ExitRequest::PrintStructure() const {
  std::cout << "ExitRequest" << std::endl;
}

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

}
