#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <memory>

#include "User.hpp"
#include "HSMP/HSMPRequest.hpp"
#include "HSMP/HSMPResponse.hpp"

const int Klenght = 50;
bool login_correct = false;

std::shared_ptr<HSMP::ClientRequest> CreateRequest();

void WaitForResponses(int connection_socket) {
  char first_char;

  while (1) {

    // PHASE 04
    auto res = std::shared_ptr<HSMP::ServerResponse>();
    res = HSMP::ProcessResponse(connection_socket, first_char);
    res->PrintStructure();

    if (res->type() == HSMP::kLoginResponse) {
      login_correct = true;
    }

    if (first_char == '\0') {
      std::cout << "CONNECTION TO SERVER ENDED. ANY FOLLOWING MESSAGES WILL FAIL" <<
                std::endl;
      close(connection_socket);
      break;
    }

  }
}

int Connect(std::string ip, int port) {
  int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (sock == -1) {
    perror("Couldn't create socket");
    exit(EXIT_FAILURE);
  }

  sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(sockaddr_in));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  int Res = inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);

  if (Res < 0) {
    perror("Error: First parameter is not a valid address family");
    close(sock);
    exit(EXIT_FAILURE);
  }
  else if (Res == 0) {
    perror("char string (second parameter does not contain valid ip address");
    close(sock);
    exit(EXIT_FAILURE);
  }

  if (connect(sock, (const sockaddr*)&server_addr, sizeof(sockaddr_in)) == -1) {
    perror("Connection failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  return sock;
}

int main(void) {
  int connection_socket = Connect("127.0.0.1", 45000);
  std::thread response_listener(WaitForResponses, connection_socket);

  while(!login_correct) {

    auto lreq = std::make_shared<HSMP::LoginRequest>();
    std::cout << "Creating Login Request" << '\n';

    std::cout << "What is the username: ";
    getline(std::cin, lreq->user);
    std::cout << "What is the password: ";
    getline(std::cin, lreq->passwd);

    lreq->tam_user = lreq->user.size();
    lreq->tam_passwd = lreq->passwd.size();

    send(connection_socket, lreq->ParseToCharBuffer(), Klenght, 0);
    std::this_thread::sleep_for(std::chrono::seconds(1));

  }
  system("clear");
  while (1) {
    auto req = std::shared_ptr<HSMP::ClientRequest>();
    req = CreateRequest();
    
    if (req) {
      //printf("mensaje parseado: %s\n", req->ParseToCharBuffer());
      send(connection_socket, req->ParseToCharBuffer(), Klenght, 0);
    }
    if (req->type() == HSMP::kExitRequest) {
      break;
    }
  }

  shutdown(connection_socket, SHUT_RDWR);
  close(connection_socket);
  return 0;
}

std::shared_ptr<HSMP::ClientRequest> CreateRequest() {
  char accion;
  printf("What do you want to do? [i: lista] [m: message] [b: broadcast] [u: uploadFile] [x: exit]\n");

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
