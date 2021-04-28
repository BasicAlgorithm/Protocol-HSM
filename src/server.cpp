#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <memory>

#include "User.hpp"
#include "HSMP/HSMPRequest.hpp"
#include "HSMP/HSMPResponse.hpp"

const int Klenght = 50;


auto firsts_users = { User("127.0.0.1","Joaquin","gaa"),
                      User("127.0.0.1","Mateo","gee"),
                      User("127.0.0.1","Miguel","gii")
                    };

auto users = std::make_shared<std::list<User>>(firsts_users);

std::shared_ptr<HSMP::ServerResponse> CreateResponse(std::shared_ptr<HSMP::ClientRequest> request);

int main () {
  struct sockaddr_in stSockAddr;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (-1 == SocketFD) {
    perror("can not create socket");
    exit(EXIT_FAILURE);
  }

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(45000);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;

  if (-1 == bind(SocketFD, (const struct sockaddr*)&stSockAddr,
                 sizeof(struct sockaddr_in))) {
    perror("error bind failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  if (-1 == listen(SocketFD, 10)) {
    perror("error listen failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  printf("\nHSMP Waiting for client");
  fflush(stdout);

  while (true) {
    int ConnectFD = accept(SocketFD, NULL, NULL);

    if (0 > ConnectFD) {
      perror("error accept failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }

    std::shared_ptr<HSMP::ClientRequest> req = HSMP::ProcessRequest(ConnectFD);
    req->PrintStructure();

    // auto res = std::shared_ptr<HSMP::ServerResponse>();
    // res = CreateResponse(req);
    // write(ConnectFD, res->ParseToCharBuffer(), Klenght);

    shutdown(ConnectFD, SHUT_RDWR);
    close(ConnectFD);
  }

  close(SocketFD);
  return 0;
}

std::shared_ptr<HSMP::ServerResponse> CreateResponse(std::shared_ptr<HSMP::ClientRequest> request) {
  
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
