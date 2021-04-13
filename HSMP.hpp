#pragma once
#include <string>

struct HSMP {

  struct ClientRequest {
    struct Login {
      char accion; // l
      short int tam_user;
      short int tam_passwd;
      char* user;
      char* passwd;
    };
    struct Lista {
      char accion; // i
    };
    struct Message {
      char accion; // m
      char tam_msg[3];
      char tam_destinatario[2];
      char* msg;
      char* destinatario;
    };
    struct Broadcast {};
    struct UploadFile {};
    struct File_AN {
      char accion; // f
      char tam_remitente[2];
      char* remitente;
    };
    struct Exit {
      char accion; // x
    };
  };

  struct ServerResponse {
    struct Login{
      char accion; // L
      char ok[2];
    };
    struct Lista {
      char accion; // I
      char num_users[2];
      char tam_user_name[2];
      char* user_name;
    };
    struct Message {
      char accion; // M
      char tam_msg[3];
      char tam_remitente[2];
      char* msg;
      char* remitente;
    };
    struct Broadcast {};
    struct UploadFile {};
    struct File_AN {
      char accion; // F
      char tam_usuario[2];
      char* usuario;
    };
    struct Exit {
      char accion; // X
    };
    struct Error {
      char accion; // E
      char message[20];
    };
  };

  static void ProcessRequest(char* buffer);
  static void ProcessResponse(char* buffer); //read -->
  static void SendRequest(struct ClientRequest request);// --> write
  static void SendResponse(struct ServerResponse response);
  
  static void ProcessLoginRequest();
  static void ProcessListaRequest();
  static void ProcessMessageRequest();
  static void ProcessBroadcastRequest();
  static void ProcessUploadFileRequest();
  static void ProcessFile_ANRequest();
  static void ProcessExitRequest();

  static void ProcessLoginResponse();
  static void ProcessListaResponse();
  static void ProcessMessageResponse();
  static void ProcessBroadcastResponse();
  static void ProcessUploadFileResponse();
  static void ProcessFile_ANResponse();
  static void ProcessExitResponse();
  static void ProcessErrorResponse();

};
