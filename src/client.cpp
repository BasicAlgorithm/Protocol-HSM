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

#include "HSMP/HSMPRequest.hpp"
#include "HSMP/HSMPResponse.hpp"

const int Klenght = 50;
std::shared_ptr<HSMP::ClientRequest> CreateRequest();

void WaitForResponses(int connection_socket) {
  char buffer[1000];

  while (1) {
    bzero(buffer, 1000);
    recv(connection_socket, buffer, 1000, 0);

    if (buffer[0] == '\0') {
      std::cout << "CONNECTION TO SERVER ENDED. ANY FOLLOWING MESSAGES WILL FAIL" <<
                std::endl;
      close(connection_socket);
      break;
    }

    printf("Server: %s\n", buffer);
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

  while (1) {
    auto req = std::shared_ptr<HSMP::ClientRequest>();
    req = CreateRequest();
    printf("mensaje parseado: %s\n", req->ParseToCharBuffer());
    send(connection_socket, req->ParseToCharBuffer(), Klenght, 0);
  }

  shutdown(connection_socket, SHUT_RDWR);
  close(connection_socket);
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
