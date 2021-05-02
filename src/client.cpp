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
              // REQUEST CLIENT -> SERVER [PHASE 1]
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
              // RESPONSE SERVER -> CLIENT [PHASE 4]
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
  printf("What do you want to do? [l] [i] [m] [b] [u] [f] [x]\n");

  std::cin >> accion;
  std::cin.ignore();

  switch (accion) {
    case 'l': {
      auto lreq = std::make_shared<HSMP::LoginRequest>();
      std::cout << "Creating Login Request" << '\n';

      std::cout << "What is the username: ";
      getline(std::cin, lreq->user);
      std::cout << "What is the password: ";
      getline(std::cin, lreq->passwd);

      lreq->tam_user = lreq->user.size();
      lreq->tam_passwd = lreq->passwd.size();

      return lreq;
    }

    case 'i': {
      auto ireq = std::make_shared<HSMP::ListaRequest>();
      std::cout << "Creating Lista Request" << '\n';
      return ireq;
    }

    case 'm': {
      auto mreq = std::make_shared<HSMP::MessageRequest>();

      std::cout << "Creating Message Request" << '\n';

      std::cout << "What is the message: ";
      getline(std::cin, mreq->msg);
      std::cout << "Who is the recipient: ";
      getline(std::cin, mreq->destinatario);

      mreq->tam_msg = mreq->msg.size();
      mreq->tam_destinatario = mreq->destinatario.size();

      return mreq;
    }

    case 'b': {
      auto breq = std::make_shared<HSMP::BroadcastRequest>();
      std::cout << "Creating Broadcast Request" << '\n';

      std::cout << "What is the message to send: ";
      getline(std::cin, breq->msg);

      return breq;
    }

    case 'u': {
      auto ureq = std::make_shared<HSMP::UploadFileRequest>();
      std::cout << "Creating UploadFile Request" << '\n';

      std::cout << "What is the filename: ";
      getline(std::cin, ureq->file_name);
      ureq->tam_file_name = ureq->file_name.size();

      ureq->file_data = new char[2];
      ureq->file_data[0] = '0';
      ureq->file_data[1] = '\0';
      ureq->tam_file_data = 0;

      std::cout << "What is the receptor's name: ";
      getline(std::cin, ureq->destinatario);
      ureq->tam_destinatario = ureq->destinatario.size();

      return ureq;
    }

    case 'f': {
      auto freq = std::make_shared<HSMP::File_ANRequest>();
      std::cout << "Creating FileAcceptReject Request" << '\n';

      std::cout << "Who is the recipient: ";
      getline(std::cin, freq->remitente);
      freq->tam_remitente = freq->remitente.size();

      return freq;
    }

    case 'x': {
      auto xreq = std::make_shared<HSMP::ExitRequest>();
      std::cout << "Creating Exit Request" << '\n';
      return xreq;
    }

    default: {
      std::cout << "Wrong input" << std::endl;
      return nullptr;
    }
  }
}
