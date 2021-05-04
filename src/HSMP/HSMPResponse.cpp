#include "HSMPResponse.hpp"
#include "utils/base64.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>

namespace HSMP {

void LoginResponse::PrintStructure() const {
  std::cout << "LoginResponse:\n";
  //std::cout << "\tLogin state: " << this->ok << std::endl;
  std::cout << "\tBienvenido" << std::endl;
}

char *LoginResponse::ParseToCharBuffer() const {
  std::string parsed_structure("L");

  parsed_structure += this->ok;

  char *buffer= new char[parsed_structure.length() + 1];
  std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(),
                       0);
  buffer[length]='\0';

  //std::cout << "to send: " << buffer << std::endl;

  return buffer;
}

void ListaResponse::PrintStructure() const {
  std::cout << "ListaResponse:\n";
  if (this->num_users == 0) {
    std::cout << "\tnobody_online" << std::endl;
    return;
  }
  std::cout << "\tnumber of users: " << this->num_users << "\n";

  for (int i = 0; i < num_users; ++i)
    std::cout << "\tUser #" << i << ": " << this->user_names[i] << "\n";
}

char *ListaResponse::ParseToCharBuffer() const {
  // I 03 11 03 05 Santisteban Lee Peter I 03 03 02 03 Jon Wu Lee
  std::string parsed_structure("I");

  if (this->num_users <= 9)
      parsed_structure += "0";

  parsed_structure += std::to_string(this->num_users);

  for (auto &it : tam_user_names) {
    
    if (it <= 9)
      parsed_structure += "0";
    
    parsed_structure += std::to_string(it);

  }

  for (auto &it : user_names) {
    
    parsed_structure += it;

  }

  char *buffer= new char[parsed_structure.length() + 1];
  std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(),
                       0);
  buffer[length]='\0';

  //std::cout << "to send: " << buffer << std::endl;

  return buffer;
}

void MessageResponse::PrintStructure() const {
  //std::cout << "MessageResponse:\n";
  //std::cout << "\ttam_msg: " << this->tam_msg << "\n";
  //std::cout << "\ttam_remitente: " << this->tam_remitente << "\n";
  //std::cout << "\tmsg: " << this->msg << "\n";
  //std::cout << "\tremitente: " << this->remitente << std::endl;
  std::cout << "New message from " << this->remitente << ": " << this->msg << std::endl;
}

char *MessageResponse::ParseToCharBuffer() const {
  
  std::string parsed_structure("M");

  if (this->tam_msg <= 99) {
    parsed_structure += "0";

    if (this->tam_msg <= 9)
      parsed_structure += "0";
  }

  parsed_structure += std::to_string(this->tam_msg);

  if (this->tam_remitente <= 9)
      parsed_structure += "0";

  parsed_structure += std::to_string(this->tam_remitente);

  parsed_structure += this->msg;
  parsed_structure += this->remitente;


  char *buffer= new char[parsed_structure.length() + 1];
  std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(),
                       0);
  buffer[length]='\0';
  
  //std::cout << "to send: " << buffer << std::endl;

  return buffer;
}

void BroadcastResponse::PrintStructure() const {
  //std::cout << "BroadcastResponse:\n";
  //std::cout << "\tmensaje tam: " << this->tam_msg << "\n";
  //std::cout << "\tmensaje: " << this->msg << "\n";
  //std::cout << "\tremitente tam: " << this->tam_remitente << "\n";
  //std::cout << "\tremitente: " << this->remitente << std::endl;
  std::cout << "New broadcast message from " << this->remitente << ": " << this->msg << std::endl;
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
  //std::cout << "\tfile name tam: " << this->tam_file_name << "\n";
  std::cout << "\tfile: " << this->file_name << "\n";
  //std::cout << "\tfile tam: " << this-> tam_file_data << "\n";
  //std::cout << "\tfile_data: " << this-> file_data << "\n";
  //std::cout << "\tremitente tam: " << this-> tam_remitente << "\n";
  std::cout << "\tremitente: " << this->remitente << std::endl;
  Base64Formatter formatter;
  std::ofstream file(this->file_name, std::ios::binary);
  file << formatter.decode(this->file_data);
  file.close();
  std::cout << "File saved locally!\n";
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

char *ExitResponse::ParseToCharBuffer() const {
  std::string parsed_structure("X");

  char *buffer= new char[parsed_structure.length() + 1];
  std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(),
                       0);
  buffer[length]='\0';
  return buffer;

}

void ErrorResponse::PrintStructure() const {
  std::cout << "SERVER ERROR:\n";
  std::cout << "\t[Error type: " << this->message << std::endl;
}

char *ErrorResponse::ParseToCharBuffer() const {
  std::string parsed_structure("E");

  parsed_structure += this->message;

  char *buffer= new char[parsed_structure.length() + 1];
  std::size_t length = parsed_structure.copy(buffer, parsed_structure.length(),
                       0);
  buffer[length]='\0';
  return buffer;

}

std::shared_ptr<ServerResponse> ProcessResponse(int connectionFD) {
  
  char buffer[1000] = {0};
  bzero(buffer, 1000);
  recv(connectionFD, buffer, 1000, 0);
  //printf("Server: %s\n", buffer);
  // cundo el buffer es muy grande -> crash!
  
  if (buffer[0] == '\0'){
    return nullptr;
  }

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
      // I 5 05 06 06 06 06 Mateo Miguel ElPepe guest1 guest2
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

      std::string file_data;
      int max_size = Ures->tam_file_data;
      int current_size = std::min(max_size, 1000 - 16 - Ures->tam_file_name);
      file_data = s.substr(16 + Ures->tam_file_name, current_size);
      max_size -= current_size;
      if (max_size) {
        while (max_size) {
          recv(connectionFD, buffer, 1000, 0);
          s = buffer;
          current_size = std::min(max_size, 1000);
          file_data += s.substr(0, current_size);
          max_size -= current_size;
        }
        Ures->remitente = s.substr(current_size,
                                   Ures->tam_remitente);
      }
      else
        Ures->remitente = s.substr(16 + Ures->tam_file_name + Ures->tam_file_data,
                                   Ures->tam_remitente);

      Ures->file_data = file_data;
      
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

    case '\0': {
      return nullptr;
    }

    default:
      return nullptr;
  }
}

}
