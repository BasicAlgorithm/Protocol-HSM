#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <iomanip>
#include <algorithm>

#include "User.hpp"
#include "HSMP/HSMPRequest.hpp"
#include "HSMP/HSMPResponse.hpp"

const int KMaxLogs = 8;
std::mutex mtx;

const std::string kMessageWrongCredentials        ("[SERVER: wrongCrednt");
const std::string kMessageErrorPersonDisconnected ("[SERVER: prsonDiscon");
const std::string kMessageErrorPersonDontExist    ("[SERVER: persNoExist");
const std::string KMessageWellHacked              ("[SERVER: wellHacked!");
const std::string KMessageAlreadyOnline           ("[SERVER: alreadOnlin");

auto firsts_users = { User("Joaquin", "gaa"),
                      User("Mateo", "gee"),
                      User("Miguel", "gii"),
                      User("ElPepe", "ucsp"),
                      User("guest1", "guest1"),
                      User("guest2", "guest2"),
};

auto users = std::make_shared<std::list<User>>(firsts_users);
std::list<std::string> logs;

std::shared_ptr<HSMP::ServerResponse> CreateResponse(
    std::shared_ptr<HSMP::ClientRequest> request, std::shared_ptr<User*> current_user, 
    int &destinatario_FD);

void PrintScreenServer() {
  mtx.lock();
  system("clear");
  std::cout << "+-------------------------------------------+" << std::endl;
  std::cout << "|               HSMProtocol                 |" << std::endl;
  std::cout << "+-------------------------------------------+" << std::endl;
  std::cout << "|                All User                   |" << std::endl;
  std::cout << "+-------------------------------------------+" << std::endl;

  for (User &user : *users)
    std::cout << "|" << std::setw(10) << user.GetName() << std::setw(34) << "|" << std::endl;

  std::cout << "+-------------------------------------------+" << std::endl;
  std::cout << "|                Active Users               |" << std::endl;
  std::cout << "+-------------------------------------------+" << std::endl;

  std::cout << std::setw(1) << "| " 
            << std::setw(10) << "USER"
            << std::setw(15) << "IP"
            << std::setw(8) << "FD"
            << std::setw(8) << "QC" << " |" << std::endl;

  for (User &user : *users) {
    if (user.IsOnline()) std::cout <<  "| " 
                                    << std::setw(10) << user.GetName() 
                                    << std::setw(15) << user.GetIp()
                                    << std::setw(8) << user.GetFileDescriptor()
                                    << std::setw(8) << user.GetQuantityOfConnections() 
                                    << " |" << std::endl;
  }

  std::cout << "+-------------------------------------------+" << std::endl;
  std::cout << "|                Last Request               |" << std::endl;
  std::cout << "+-------------------------------------------+" << std::endl;
  if (logs.size() == 0) std::cout << "|\tno request yet"  << std::setw(23) << "" << std::endl;
  if (logs.size() >= KMaxLogs) logs.pop_front();
  for (std::string &log : logs)
    std::cout << "| " << std::setw(24) << log << std::setw(8) << "" << std::endl;

  std::cout << "+-------------------------------------------+" << std::endl;
  mtx.unlock();
}

void AttendConnection(int client_socket, sockaddr_in client_addr) {

  auto current_user = std::make_shared<User*>();
  int destinatario_FD;
  std::string ip = inet_ntoa(client_addr.sin_addr);
  int port = ntohs(client_addr.sin_port);

  while (1) {

    // PHASE 02
    auto req = HSMP::ProcessRequest(client_socket, logs/*, current_user*/);

    // This happen if client shut down or disconnect
    if (req == nullptr) {
      (*current_user)->SetOffline();
      PrintScreenServer();
      break;
    }

    // PHASE 03
    auto res = std::shared_ptr<HSMP::ServerResponse>();
    res = CreateResponse(req, current_user, destinatario_FD);

    if (res->type() == HSMP::kLoginResponse) {
      (*current_user)->SetIP(ip);
      (*current_user)->SetFileDescriptor(client_socket);
      (*current_user)->OneMoreConnection();
      (*current_user)->SetOnline();
      PrintScreenServer();
    }

    if (res->type() == HSMP::kBroadcastResponse) {
      for (User &listed_user : *users) {
        if (listed_user.GetName() == (*current_user)->GetName()) continue;
        if (listed_user.IsOnline()) {
          send(listed_user.GetFileDescriptor(), res->ParseToCharBuffer(), strlen(res->ParseToCharBuffer()), 0);
        }
      }
      continue;
    }

    if (res->type() == HSMP::kMessageResponse) {
      send(destinatario_FD, res->ParseToCharBuffer(), strlen(res->ParseToCharBuffer()), 0);
      continue;
    }

    if (res->type() == HSMP::kExitResponse) {
      send(client_socket, res->ParseToCharBuffer(), strlen(res->ParseToCharBuffer()), 0);
      PrintScreenServer();
      break;
    }
    
    send(client_socket, res->ParseToCharBuffer(), strlen(res->ParseToCharBuffer()), 0);
    PrintScreenServer();
  }

  shutdown(client_socket, SHUT_RDWR);
  close(client_socket);
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

  //std::cout << "Listening on port " << port << std::endl;
  PrintScreenServer();

  while (1) {
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_socket = accept(sock, (sockaddr*)&client_addr, &addr_len);
    logs.push_back("Accepted connection FD " + std::to_string(client_socket));

    PrintScreenServer();

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

  Listen(45000);

  return 0;
}

std::shared_ptr<HSMP::ServerResponse> CreateResponse(
  std::shared_ptr<HSMP::ClientRequest> request,
  std::shared_ptr<User*> current_user,
  int &destinatario_FD) {

  switch (request->type()) {

    case HSMP::RequestType::kLoginRequest: {

      auto lreq = std::static_pointer_cast<HSMP::LoginRequest>(request);
      auto lres = std::make_shared<HSMP::LoginResponse>();

      for (User &user : *users) {
        if (user.GetName() == lreq->user) {
          if (user.GetPassword() == lreq->passwd) {
            if (user.IsOnline()){
              auto eres = std::make_shared<HSMP::ErrorResponse>();
              eres->message = KMessageWellHacked; // joke answer
              //eres->message = KMessageAlreadyOnline; // serious answer
              return eres;
            } else {
              lres->ok = "ok";
              *current_user = &user;
              return lres;
            }
          }
          auto eres = std::make_shared<HSMP::ErrorResponse>();
          eres->message = kMessageWrongCredentials;
          return eres;
        }
      }
      auto eres = std::make_shared<HSMP::ErrorResponse>();
      eres->message = kMessageWrongCredentials;
      return eres;
    }

    case HSMP::RequestType::kListaRequest: {
      auto ires = std::make_shared<HSMP::ListaResponse>();
      for (User &listed_user : *users) {
        if (listed_user.GetName() == (*current_user)->GetName()) continue;
        if (listed_user.IsOnline()) {
          ires->tam_user_names.push_back(listed_user.GetName().size());
          ires->user_names.push_back(listed_user.GetName());
        }
      }
      ires->num_users = ires->user_names.size();
      return ires;
    }

    case HSMP::RequestType::kMessageRequest: {
      auto mreq = std::static_pointer_cast<HSMP::MessageRequest>(request);
      for (User &listed_user : *users) {
        if (listed_user.GetName() == mreq->destinatario) {
          if (listed_user.IsOnline()) {
            auto mres = std::make_shared<HSMP::MessageResponse>();
            mres->tam_msg = mreq->tam_msg;
            mres->msg = mreq->msg;
            mres->tam_remitente = (*current_user)->GetName().size();
            mres->remitente = (*current_user)->GetName();
            destinatario_FD = listed_user.GetFileDescriptor();
            return mres;
          } else {
            auto eres = std::make_shared<HSMP::ErrorResponse>();
            eres->message = kMessageErrorPersonDisconnected;
            return eres;
          }
        }
      }
      auto eres = std::make_shared<HSMP::ErrorResponse>();
      eres->message = kMessageErrorPersonDontExist;
      return eres;
    }

    case HSMP::RequestType::kBroadcastRequest: {
      auto breq = std::static_pointer_cast<HSMP::BroadcastRequest>(request);
      auto bres = std::make_shared<HSMP::BroadcastResponse>();
      bres->tam_msg = breq->tam_msg;
      bres->msg = breq->msg;
      bres->tam_remitente = (*current_user)->GetName().size();
      bres->remitente = (*current_user)->GetName();
      return bres;
    }

    case HSMP::RequestType::kUploadFileRequest: {
      auto ureq = std::static_pointer_cast<HSMP::UploadFileRequest>(request);

      for (User &listed_user : *users) {
        if (listed_user.GetName() == ureq->destinatario) {
          if (listed_user.IsOnline()) {
            auto ures = std::make_shared<HSMP::UploadFileResponse>();
            ures->tam_file_name = ureq->tam_file_name;
            ures->tam_file_data = ureq->tam_file_data;
            ures->tam_remitente = (*current_user)->GetName().size();
            ures->file_name = ureq->file_name;
            ures->remitente = (*current_user)->GetName();
            strcpy(ures->file_data, ureq->file_data);
            return ures;
          } else {
            auto eres = std::make_shared<HSMP::ErrorResponse>();
            eres->message = kMessageErrorPersonDisconnected;
            return eres;
          }
        }
      }

      auto eres = std::make_shared<HSMP::ErrorResponse>();
      eres->message = kMessageErrorPersonDontExist;
      return eres;
    }

      // UploadRequest deberia devolver un File_ANResponse al destinatario
      // File_ANRequest deberia devolver un UploadResponse al mismo user que hace el request
 
    case HSMP::RequestType::kFile_ANRequest: {
      auto freq = std::static_pointer_cast<HSMP::File_ANRequest>(request);
      auto fres = std::make_shared<HSMP::File_ANResponse>();
      fres->tam_user_name = (*current_user)->GetName().size();
      fres->user_name = (*current_user)->GetName();
      return fres;
    }

    case HSMP::RequestType::kExitRequest: {
      auto xres = std::make_shared<HSMP::ExitResponse>();
      (*current_user)->SetOffline();
      return xres;
    }

    default: {
      std::cout << "This won't be never print" << std::endl;
      return nullptr;
    }
  }
}
