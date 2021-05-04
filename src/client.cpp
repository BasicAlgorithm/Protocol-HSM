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
#include <fstream>

#include "User.hpp"
#include "HSMP/HSMPRequest.hpp"
#include "HSMP/HSMPResponse.hpp"
#include "utils/base64.hpp"

bool KLoginAccepted = false;
bool KWaitingLoginResponse = true;
bool KConnectedWithserver = true;

std::shared_ptr<HSMP::ClientRequest> CreateRequest();

void WaitForResponses(int connection_socket) {
  while (1) {
    // PHASE 04
    auto res = std::shared_ptr<HSMP::ServerResponse>();

    res = HSMP::ProcessResponse(connection_socket);

    // This happen if server shut down or disconnect
    if (res == nullptr) {
      std::cout <<
                "\n[SERVER: Connection to server ended, sorry for any inconvenient]" <<
                std::endl;
      KConnectedWithserver = false;
      break;
    }

    // To break login infinity loop
    if (res->type() == HSMP::kLoginResponse) {
      KLoginAccepted = true;
    }

    res->PrintStructure();

    // To break login infinity loop
    if (res->type() == HSMP::kLoginResponse)
      KLoginAccepted = true;

    if (res->type() == HSMP::kExitResponse)
      break;

    KWaitingLoginResponse = false;
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
    perror("Invalid IP address.");
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
  std::string server_ip = "127.0.0.1";
  std::cout << "Server IP (default=127.0.0.1): ";
  std::cin >> server_ip;

  int port = 45000;
  std::cout << "Port (default=45000): ";
  std::cin >> port;

  int connection_socket = Connect(server_ip, port);
  std::thread response_listener(WaitForResponses, connection_socket);

  std::cin.ignore();
  while (!KLoginAccepted && KConnectedWithserver) {
    auto lreq = std::make_shared<HSMP::LoginRequest>();
    std::cout << "Creating Login Request" << '\n';

    std::cout << "What is your username: ";
    getline(std::cin, lreq->user);
    std::cout << "What is your password: ";
    getline(std::cin, lreq->passwd);

    lreq->tam_user = lreq->user.size();
    lreq->tam_passwd = lreq->passwd.size();

    KWaitingLoginResponse = true;

    send(connection_socket, lreq->ParseToCharBuffer(),
         strlen(lreq->ParseToCharBuffer()), 0);

    while (KWaitingLoginResponse);
  }

  system("clear");

  while (KConnectedWithserver) {
    // To intent orden print
    std::this_thread::sleep_for(std::chrono::milliseconds(120));

    auto req = std::shared_ptr<HSMP::ClientRequest>();
    req = CreateRequest();

    if ((req == nullptr) || !KConnectedWithserver)
      continue;

    if (req) {
      //printf("mensaje parseado: %s\n", req->ParseToCharBuffer());
      send(connection_socket, req->ParseToCharBuffer(),
           strlen(req->ParseToCharBuffer()), 0);
    }

    if (req->type() == HSMP::kExitRequest)
      break;

  }

  std::cout << "\n Disconnecting... ";

  response_listener.join();

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

      std::cout << "What is the file path: ";
      std::string path;
      getline(std::cin, path);

      std::ifstream file(path, std::ios::binary | std::ios::ate);
      if (file.is_open()) {
        Base64Formatter formatter;
        std::streampos data_size;
        char* memblock;

        data_size = file.tellg();
        memblock = new char[data_size];
        file.seekg (0, std::ios::beg);
        file.read (memblock, data_size);
        file.close();

        ureq->file_data = formatter.encode(memblock, data_size);
        ureq->tam_file_data = ureq->file_data.size();

        delete[] memblock;
      }
      else {
        std::cout << "Unable to open file\n";
        return nullptr;
      }

      std::cout << "What is the file name: ";
      getline(std::cin, ureq->file_name);
      ureq->tam_file_name = ureq->file_name.size();

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
      std::cout << "[SERVER: wrong_key_request]" << std::endl;
      return nullptr;
    }
  }
}
