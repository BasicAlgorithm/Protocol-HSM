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
  std::shared_ptr<HSMP::ClientRequest> request);

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
  std::shared_ptr<HSMP::ClientRequest> request) {

  switch (request->type()) {

    case HSMP::RequestType::kLoginRequest: {

      auto lres = std::make_shared<HSMP::LoginResponse>();

      return lres;
    }

    case HSMP::RequestType::kListaRequest: {
      auto ires = std::make_shared<HSMP::ListaResponse>();

      return ires;
    }

    case HSMP::RequestType::kMessageRequest: {
      auto mres = std::make_shared<HSMP::MessageResponse>();

      return mres;
    }

    case HSMP::RequestType::kBroadcastRequest: {
      auto bres = std::make_shared<HSMP::BroadcastResponse>();
      // write(ConnectFD, res->ParseToCharBuffer(), Klenght);
      return bres;
    }

    case HSMP::RequestType::kUploadFileRequest: {
      auto ures = std::make_shared<HSMP::UploadFileResponse>();

      return ures;
    }

    case HSMP::RequestType::kFile_ANRequest: {
      auto fres = std::make_shared<HSMP::File_ANResponse>();

      return fres;
    }

    case HSMP::RequestType::kExitRequest: {
      auto xres = std::make_shared<HSMP::ExitResponse>();

      return xres;
    }

    default: {
      std::cout << "wrong input" << std::endl;
      return nullptr;
    }
  }
}
