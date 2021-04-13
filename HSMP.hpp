#pragma once

struct HSMP {

  struct ClientRequest {
    struct Login {
      char accion; // l
      char tam_user[2];
      char tam_passwd[2];
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
    struct Login {
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

};
