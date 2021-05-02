#include "HSMPRequest.hpp"

namespace HSMP {

void LoginRequest::PrintStructure() const {
  std::cout << "LoginRequest:\n";
  std::cout << "\ttam_user: " << this->tam_user << "\n";
  std::cout << "\ttam_passwd: " << this->tam_passwd << "\n";
  std::cout << "\tuser: " << this->user << "\n";
  std::cout << "\tpasswd: " << this->passwd << std::endl;
}

char* LoginRequest::ParseToCharBuffer() const {
  std::string parsed_structure ("l");

  if (this->tam_user <= 9)
    parsed_structure+="0";

  parsed_structure += std::to_string(this->tam_user);

  if (this->tam_passwd <= 9)
    parsed_structure+="0";

  parsed_structure += std::to_string(this->tam_passwd);

  // adding body
  parsed_structure += this->user + this->passwd;
  char* buffer= new char[parsed_structure.length() + 1];
  std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(),
                       0);
  buffer[length] = '\0';

  std::cout << "\tLoginRequest sending to server: " << buffer << std::endl;

  return buffer;
}

void ListaRequest::PrintStructure() const {
  std::cout << "ListaRequest" << std::endl;
}

char* ListaRequest::ParseToCharBuffer() const {
  char* buffer = new char[2];
  buffer[0] = 'i';
  buffer[1] = '\0';
  // std::cout << "\tListaRequest sending to server: " << buffer << std::endl;

  return buffer;
}

void MessageRequest::PrintStructure() const {
  std::cout << "MessageRequest:\n";
  std::cout << "\ttam_msg: " << this->tam_msg << "\n";
  std::cout << "\ttam_destinatario: " << this->tam_destinatario << "\n";
  std::cout << "\tmsg: " << this->msg << "\n";
  std::cout << "\tdestinatario: " << this->destinatario << std::endl;
}

char* MessageRequest::ParseToCharBuffer() const {
  std::string parsed_structure("m");

  if (this->tam_msg <= 9)
    parsed_structure += "00";
  else if (this->tam_msg <= 99)
    parsed_structure += "0";

  parsed_structure += std::to_string(this->tam_msg);

  if (this->tam_destinatario <= 9)
    parsed_structure += "0";

  parsed_structure += std::to_string(this->tam_destinatario);

  parsed_structure += this->msg + this->destinatario;
  // std::cout << "\tMessageRequest sending to server: " << parsed_structure <<
  // std::endl;

  char* buffer = new char[parsed_structure.size() + 1];
  std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(),
                       0);
  buffer[length] = '\0';
  return buffer;
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
  std::cout << "\ttam_destinatario: " << this->tam_destinatario << "\n";
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

char* File_ANRequest::ParseToCharBuffer() const {
  std::string parsed_structure("f");

  if (this->tam_remitente < 9)
    parsed_structure += "0";

  parsed_structure += std::to_string(this->tam_remitente);

  parsed_structure += this->remitente;
  // std::cout << "\tFile_ANRequest sending to server: " << parsed_structure <<
  // std::endl;

  char* buffer= new char[parsed_structure.length() + 1];
  std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(),
                       0);
  buffer[length]='\0';
  return buffer;
}

void ExitRequest::PrintStructure() const {
  std::cout << "ExitRequest" << std::endl;
}

char* ExitRequest::ParseToCharBuffer() const {
  char* buffer = new char[2];
  buffer[0] = 'x';
  buffer[1] = '\0';
  // std::cout << "\tExitRequest sending to server: " << buffer << std::endl;
  return buffer;
}

std::shared_ptr<ClientRequest> ProcessRequest(int connection_socket) {
  char buffer[1000] = {0};
  recv(connection_socket, buffer, 1000, 0);

  if (buffer[0] == '\0')
    return std::make_shared<ExitRequest>();

  printf("\n\tmensaje recibido: %s\n", buffer);

  char action = buffer[0];
  std::string s;

  switch (action) {
    case 'l': {
      auto lreq = std::make_shared<LoginRequest>();

      lreq->tam_user = stoi(s.substr(1, 2));
      lreq->tam_passwd = stoi(s.substr(3, 2));

      lreq->user = s.substr(5, lreq->tam_user);
      lreq->passwd = s.substr(5 + lreq->tam_user, lreq->tam_passwd);

      return lreq;
    }

    case 'i': {
      auto ireq = std::make_shared<ListaRequest>();
      return ireq;
    }

    case 'm': {
      auto mreq = std::make_shared<MessageRequest>();

      mreq->tam_msg = stoi(s.substr(1, 3));
      mreq->tam_destinatario = stoi(s.substr(4, 2));

      mreq->msg = s.substr(6, mreq->tam_msg);
      mreq->destinatario = s.substr(6 + mreq->tam_msg, mreq->tam_destinatario);

      return mreq;
    }

    case 'b': {
      auto breq = std::make_shared<BroadcastRequest>();

      breq->tam_msg = stoi(s.substr(1, 3));
      breq->msg = s.substr(4);

      return breq;
    }

    case 'u': {
      auto ureq = std::make_shared<UploadFileRequest>();

      ureq->tam_file_name = stoi(s.substr(1, 3));
      ureq->tam_file_data = stoi(s.substr(4, 10));
      ureq->tam_destinatario = stoi(s.substr(14, 2));

      ureq->file_name = s.substr(16, ureq->tam_file_name);

      ureq->file_data = new char[ureq->tam_file_data];
      strcpy(ureq->file_data, s.substr(16 + ureq->tam_file_name,
                                       ureq->tam_file_data).c_str());

      ureq->destinatario = s.substr(16 + ureq->tam_file_name + ureq->tam_file_data,
                                    ureq->tam_destinatario);

      return ureq;
    }

    case 'f': {
      auto freq = std::make_shared<File_ANRequest>();

      freq->tam_remitente = stoi(s.substr(1, 2));
      freq->remitente = s.substr(3, freq->tam_remitente);

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
