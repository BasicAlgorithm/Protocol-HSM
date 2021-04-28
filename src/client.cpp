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

#include "HSMP/HSMPRequest.hpp"
#include "HSMP/HSMPResponse.hpp"

const int Klenght = 50;
std::shared_ptr<HSMP::ClientRequest> CreateRequest();


int main(void) {
  struct sockaddr_in stSockAddr;
  int Res;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  int n;

  if (-1 == SocketFD) {
    perror("cannot create socket");
    exit(EXIT_FAILURE);
  }

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(45000);
  Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);

  if (0 > Res) {
    perror("error: first parameter is not a valid address family");
    close(SocketFD);
    exit(EXIT_FAILURE);
  } else if (0 == Res) {
    perror("char string (second parameter does not contain valid ipaddress");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr,
                    sizeof(struct sockaddr_in))) {
    perror("connect failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }
  /*
  int connectionFD;
  while(true) {

      if (fork() == 0)
      {
          while (1)
          {
*/
              auto req = std::shared_ptr<HSMP::ClientRequest>();
              req = CreateRequest();
              printf("message parseado: %s\n",req->ParseToCharBuffer());
              n = write(SocketFD, req->ParseToCharBuffer(), Klenght);
  /*        }
      }
      else
      {
          while (1)
          {
              //connectionFD = read(SocketFD, buffer, Klenght);
              std::shared_ptr<HSMP::ServerResponse> res = HSMP::ProcessResponse(connectionFD);
              res->PrintStructure();
          }
      }
  }*/

  shutdown(SocketFD, SHUT_RDWR);

  close(SocketFD);
  return 0;
}

std::shared_ptr<HSMP::ClientRequest> CreateRequest() {

  char accion;
  printf("What do you want to do? [l] [i] [m] [b] [u] [x]\n");
  std::cin >> accion;
  std::cin.ignore();

  switch (accion) {
    case 'b': {
      auto breq = std::make_shared<HSMP::BroadcastRequest>();
      std::cout << "Creando Broadcast Request" << '\n';

      char msgs[999];
      std::cout << "what is the message to send: ";
      std::cin.getline(msgs, 999, '\n');
      breq->msg = std::string(msgs);

      return breq;
    }

    case 'u': {
      auto ureq = std::make_shared<HSMP::UploadFileRequest>();
      std::cout << "Creando UploadFile Request" << '\n';

      char buffer_name_file[999];
      std::cout << "what is the file'name: ";
      std::cin.getline(buffer_name_file, 999, '\n');
      ureq->file_name = std::string(buffer_name_file);

      char buffer_data_file[999]; // it should be 10 times 9
      std::cout << "what is the file'data: ";
      std::cin.getline(buffer_data_file, 999, '\n');
      ureq->file_data = new char[strlen(buffer_data_file)];
      strcpy(ureq->file_data, buffer_data_file);

      char buffer_name_receptor[99];
      std::cout << "what is the receptor'name: ";
      std::cin.getline(buffer_name_receptor, 99, '\n');
      ureq->destinatario = std::string(buffer_name_receptor);

      return ureq;
    }

    default: {
      std::cout << "wrong input" << std::endl;
      return nullptr;
    }
  }
}
