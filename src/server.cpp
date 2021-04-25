#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include "HSMP.hpp"

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

  while (true) {
    int ConnectFD = accept(SocketFD, NULL, NULL);

    if (0 > ConnectFD) {
      perror("error accept failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }

    std::shared_ptr<HSMP::ClientRequest> req = HSMP::ProcessRequest(ConnectFD);
    req->PrintStructure();

    char writeBuffer[30];
    std::cin >> writeBuffer;
    write(ConnectFD, writeBuffer, strlen(writeBuffer));

    shutdown(ConnectFD, SHUT_RDWR);
    close(ConnectFD);
  }

  close(SocketFD);
  return 0;
}
