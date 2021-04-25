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

void BroadcastResponse::ParseToCharBuffer() const {
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

void UploadFileResponse::PrintStructure() const {
  std::cout << "UploadFileResponse:\n";
  std::cout << "\tfile name tam: " << this->tam_file_name << "\n";
  std::cout << "\tfile: " << this-> file_name << "\n";
  std::cout << "\tfile tam: " << this-> tam_file_data << "\n";
  std::cout << "\tfile_data: " << this-> file_data << "\n";
  std::cout << "\tremitente tam: " << this-> tam_remitente << "\n";
  std::cout << "\tremitente: " << this->remitente << std::endl;
}

void UploadFileResponse::ParseToCharBuffer() const {
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

}
