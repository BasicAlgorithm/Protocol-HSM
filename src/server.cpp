#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <sys/socket.h>

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

#include "User.hpp"
#include "HSMP/HSMPRequest.hpp"
#include "HSMP/HSMPResponse.hpp"

const int Klenght = 50;

const std::string kMessageErrorPassword ("[HSMP] password_wrong");
const std::string kMessageErrorPersonDisconnected ("[HSMP] person_discon");
const std::string kMessageErrorPersonDontExist ("[HSMP] pers_no_exist");

auto firsts_users = { User("127.0.0.1", "Joaquin", "gaa"),
       User("127.0.0.1", "Mateo", "gee"),
       User("127.0.0.1", "Miguel", "gii"),
       User("127.0.0.1", "ElPepe", "ucsp"),
       User("127.0.0.1", "guest1", "guest1"),
       User("127.0.0.1", "guest2", "guest2"),
};

auto users = std::make_shared<std::list<User>>(firsts_users);
std::list<std::string> logs;

std::shared_ptr<HSMP::ServerResponse> CreateResponse(
    std::shared_ptr<HSMP::ClientRequest> request, User &current_user);

void PrintLog() {
  std::cout << "*********************************************" << std::endl;
  std::cout << "*************** HSMProtocol *****************" << std::endl;
  std::cout << "*********************************************" << std::endl;
  std::cout << "***************** All User ******************" << std::endl;
  std::cout << "*********************************************" << std::endl;

  for (std::list<User>::iterator user = users->begin(); user != users->end();
       ++user)
    std::cout << "\t" << user->GetName() << std::endl;

  std::cout << "*********************************************" << std::endl;
  std::cout << "**************** Active Users ***************" << std::endl;
  std::cout << "*********************************************" << std::endl;

  for (std::list<User>::iterator user = users->begin(); user != users->end();
       ++user) {
    if (user->IsOnline()) std::cout << "\t" << user->GetName() << std::endl;
  }

  std::cout << "*********************************************" << std::endl;
  std::cout << "************** Last Request *****************" << std::endl;
  std::cout << "*********************************************" << std::endl;

  for (std::list<std::string>::iterator log = logs.begin(); log != logs.end();
       ++log)
    std::cout << "\t" << *log << std::endl;

  std::cout << "*********************************************" << std::endl;
}

void AttendConnection(int client_socket, sockaddr_in client_addr) {
  std::cout << "Processing requests for (" << inet_ntoa(client_addr.sin_addr) <<
            ", " <<
            ntohs(client_addr.sin_port) << ") in socket " << client_socket <<
            std::endl;

  while (1) {
    auto req = HSMP::ProcessRequest(client_socket);
    req->PrintStructure();

    if (req->type() == HSMP::kExitRequest) {
      close(client_socket);
      break;
    }
    auto res = std::shared_ptr<HSMP::ServerResponse>();
    // Debe existir una comprobacion para saber quien esta mandando el request, like this:
    // for (User &sender_user : *users) {
    //    if (sender_user.GetFileDescriptor() == client_socket)
    auto user = *(users->begin());
    res = CreateResponse(req, user);
    // Una vez que se tiene el response, averiguar a quien debe mandarse.
    // e.g. Login al mismo user, Message al destinatario, ...
    
    send(client_socket, "Request received", 50, 0);
  }
}

void Listen(int port) {
  int sock;

  if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("Socket creation error!");
    exit(EXIT_FAILURE);
  }

  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(server_addr.sin_zero), 8);

  if (bind(sock, (sockaddr*)&server_addr, sizeof(sockaddr_in)) == -1) {
    perror("Socket binding error");
    close(sock);
    exit(EXIT_FAILURE);
  }

  if (listen(sock, SOMAXCONN) == -1) {
    perror("Listening failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  std::cout << "Listening on port " << port << std::endl;

  while (1) {
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_socket = accept(sock, (sockaddr*)&client_addr, &addr_len);
    std::cout << "Accepted connection on socket " << client_socket << std::endl;

    if (client_socket == -1) {
      perror("Problem with client connecting");
      close(sock);
      exit(1);
    }

    std::thread clientThread(AttendConnection, client_socket, client_addr);
    clientThread.detach();
  }

  close(sock);
}

int main () {
  logs.push_back("List from Xian");
  logs.push_back("Message from Xian to Lee");
  logs.push_back("Message from Lee to Xian");
  logs.push_back("UploadFile from Xian to Lee");
  users->begin()->SetOffline();

  Listen(45000);

  return 0;
}

std::shared_ptr<HSMP::ServerResponse> CreateResponse(
    std::shared_ptr<HSMP::ClientRequest> request, User &current_user) {

  switch (request->type()) {

    case HSMP::RequestType::kLoginRequest: {
      auto lreq = std::static_pointer_cast<HSMP::LoginRequest>(request);
      auto lres = std::make_shared<HSMP::LoginResponse>();
      if (current_user.GetName() == lreq->user) {
        if (current_user.GetPassword() == lreq->passwd) {
          current_user.SetOnline();
          lres->ok = "ok";
          return lres;
        }
        else {
          auto eres = std::make_shared<HSMP::ErrorResponse>();
          eres->message = kMessageErrorPassword;
          return eres;
        }
      }
      auto eres = std::make_shared<HSMP::ErrorResponse>();
      eres->message = kMessageErrorPersonDontExist;
      return eres;
    }

    case HSMP::RequestType::kListaRequest: {
      auto ires = std::make_shared<HSMP::ListaResponse>();
      for (User &listed_user : *users) {
        if (listed_user.IsOnline()) {
          ires->tam_user_names.push_back(listed_user.GetName().size());
          ires->user_names.push_back(listed_user.GetName());
        }
      }
      ires->num_users = ires->user_names.size();
      return ires;
    }

    case HSMP::RequestType::kMessageRequest: {
      // Se asume que las comprobaciones se hacen en AttendConnection para saber
      // a quien se debe mandar el response, y si ese usuario existe y esta conectado
      auto mreq = std::static_pointer_cast<HSMP::MessageRequest>(request);
      auto mres = std::make_shared<HSMP::MessageResponse>();
      mres->tam_msg = mreq->tam_msg;
      mres->msg = mreq->msg;
      mres->tam_remitente = current_user.GetName().size();
      mres->remitente = current_user.GetName();
      return mres;
    }

    case HSMP::RequestType::kBroadcastRequest: {
      // Se asume que el mismo mensaje se manda a todos los usuarios conectados
      auto breq = std::static_pointer_cast<HSMP::BroadcastRequest>(request);
      auto bres = std::make_shared<HSMP::BroadcastResponse>();
      bres->tam_msg = breq->tam_msg;
      bres->msg = breq->msg;
      bres->tam_remitente = current_user.GetName().size();
      bres->remitente = current_user.GetName();
      return bres;
    }

    case HSMP::RequestType::kUploadFileRequest: {
      auto ureq = std::static_pointer_cast<HSMP::UploadFileRequest>(request);
      auto ures = std::make_shared<HSMP::UploadFileResponse>();
      ures->tam_file_name = ureq->tam_file_name;
      ures->tam_file_data = ureq->tam_file_data;
      ures->tam_remitente = current_user.GetName().size();
      ures->file_name = ureq->file_name;
      ures->remitente = current_user.GetName();
      strcpy(ures->file_data, ureq->file_data);
      return ures;
    }

      // UploadRequest deberia devolver un File_ANResponse al destinatario
      // File_ANRequest deberia devolver un UploadResponse al mismo user que hace el request
 
    case HSMP::RequestType::kFile_ANRequest: {
      auto freq = std::static_pointer_cast<HSMP::File_ANRequest>(request);
      auto fres = std::make_shared<HSMP::File_ANResponse>();
      fres->tam_user_name = current_user.GetName().size();
      fres->user_name = current_user.GetName();
      return fres;
    }

    case HSMP::RequestType::kExitRequest: {
      auto xres = std::make_shared<HSMP::ExitResponse>();
      current_user.SetOffline();
      return xres;
    }

    default: {
      std::cout << "wrong input" << std::endl;
      return nullptr;
    }
  }
}
