#include "HSMPResponse.hpp"

namespace HSMP {

void LoginResponse::PrintStructure() const {
  std::cout << "LoginResponse:\n";
  std::cout << "\tLogin state: " << this->ok << std::endl;
}

void ListaResponse::PrintStructure() const {
  std::cout << "ListaResponse:\n";
  std::cout << "\tnumber of users: " << this->num_users << "\n";

  for (int i = 0; i < num_users; ++i)
    std::cout << "\tUser #" << i << ": " << this->user_names[i] << "\n";
}

void MessageResponse::PrintStructure() const {
  std::cout << "MessageResponse:\n";
  std::cout << "\ttam_msg: " << this->tam_msg << "\n";
  std::cout << "\ttam_remitente: " << this->tam_remitente << "\n";
  std::cout << "\tmsg: " << this->msg << "\n";
  std::cout << "\tremitente: " << this->remitente << std::endl;
}

void BroadcastResponse::PrintStructure() const {
  std::cout << "BroadcastResponse:\n";
  std::cout << "\tmensaje tam: " << this->tam_msg << "\n";
  std::cout << "\tmensaje: " << this->msg << "\n";
  std::cout << "\tremitente tam: " << this->tam_remitente << "\n";
  std::cout << "\tremitente: " << this->remitente << std::endl;
}

char *BroadcastResponse::ParseToCharBuffer() const {
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

  char *buffer= new char[parsed_structure.length() + 1];
  std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(),
                       0);
  buffer[length]='\0';

  
  //std::cout << "to send: " << buffer << std::endl;

  return buffer;
}

void UploadFileResponse::PrintStructure() const {
  std::cout << "UploadFileResponse:\n";
  std::cout << "\tfile name tam: " << this->tam_file_name << "\n";
  std::cout << "\tfile: " << this-> file_name << "\n";
  std::cout << "\tfile tam: " << this-> tam_file_data << "\n";
  std::cout << "\tfile_data: " << this-> file_data << "\n";
  std::cout << "\tremitente tam: " << this-> tam_remitente << "\n";
  std::cout << "\tremitente: " << this->remitente << std::endl;
}

char *UploadFileResponse::ParseToCharBuffer() const {
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

  char *buffer= new char[parsed_structure.length() + 1];
  std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(),
                       0);
  buffer[length]='\0';

  //std::cout << "to send: " << buffer << std::endl;

  return buffer;
}

void File_ANResponse::PrintStructure() const {
  std::cout << "File_ANResponse:\n";
  std::cout << "\ttam_user_name: " << this->tam_user_name << "\n";
  std::cout << "\tuser_name: " << this->user_name << std::endl;
}

void ExitResponse::PrintStructure() const {
  std::cout << "ExitResponse" << std::endl;
}

void ErrorResponse::PrintStructure() const {
  std::cout << "ErrorResponse:\n";
  std::cout << "\tmessage: " << this->message << std::endl;
}

std::shared_ptr<ServerResponse> ProcessResponse(int connectionFD) {
  char buffer[1000] = {0};
  bzero(buffer, 1000);
  recv(connectionFD, buffer, 1000, 0);

  char action = buffer[0];
  std::string s = buffer;

  switch (action) {
    case 'L': {
      auto Lres = std::make_shared<LoginResponse>();

      Lres->ok = s.substr(1,2);;

      return Lres;
    }

    case 'I': {
      // * 12 34 56 78 9*123456789 *12 3456789*123456
      // I 03 11 03 05 Santisteban Lee Peter
      auto Ires = std::make_shared<ListaResponse>();

      Ires->num_users = stoi(s.substr(1, 2));

      int index = 3;
      for (int i = 0; i < Ires->num_users; ++i, index+=2) {
        int tam_user_name = stoi(s.substr(index, 2));
        Ires->tam_user_names.push_back(tam_user_name);
      }

      for (std::vector<short int>::iterator it = Ires->tam_user_names.begin();
           it != Ires->tam_user_names.end();
            ++it) {
        Ires->user_names.push_back(s.substr(index,*it));
        index += *it;
      }

      return Ires;
    }

    case 'M': {
      // * 123 45 6789* 123456789*
      // M 005 05 hello Mateo
      auto Mres = std::make_shared<MessageResponse>();

      Mres->tam_msg = stoi(s.substr(1, 3));
      Mres->tam_remitente = stoi(s.substr(4, 2));

      Mres->msg = s.substr(6, Mres->tam_msg);
      Mres->remitente = s.substr(6 + Mres->tam_msg, Mres->tam_remitente);

      return Mres;
    }

    case 'B': {
      // * 123 45 6789*12345 6789*
      // B 010 03 HolaATodos Lee
      auto Bres = std::make_shared<BroadcastResponse>();

      Bres->tam_msg = stoi(s.substr(1, 3));
      Bres->tam_remitente = stoi(s.substr(4, 2));

      Bres->msg = s.substr(6, Bres->tam_msg);
      Bres->remitente = s.substr(6 + Bres->tam_msg, Bres->tam_remitente);

      return Bres;
    }

    case 'U': {
      // * 123 456789*123 45 6789*1 23456789*1 23456789
      // U 006 0000000010 05 MiFoto DataDeFoto Mateo
      auto Ures = std::make_shared<UploadFileResponse>();

      Ures->tam_file_name = stoi(s.substr(1, 3));
      Ures->tam_file_data = stoi(s.substr(4, 10));
      Ures->tam_remitente = stoi(s.substr(14, 2));

      Ures->file_name = s.substr(16, Ures->tam_file_name);
      Ures->file_name = s.substr(16 + Ures->tam_file_name, Ures->tam_file_data);
      Ures->remitente = s.substr(16 + Ures->tam_file_name + Ures->tam_file_data, Ures->tam_remitente);

      return Ures;
    }

    case 'F': {
      auto Fres = std::make_shared<File_ANResponse>();

      return Fres;
    }

    case 'X': {
      auto Xres = std::make_shared<ExitResponse>();
      
      return Xres;
    }

    case 'E': {
      auto Eres = std::make_shared<ErrorResponse>();

      Eres->message = s.substr(1,20);

      return Eres;
    }

    default:
      return nullptr;
  }
}

}
