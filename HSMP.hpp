#pragma once

struct HSMP {

  struct Client {
    struct Login {};

    struct Lista {};

    struct Message {
      char accion; // m
      char tam_msg[3];
      char tam_destinatario[2];
      char *msg;
      char *destinatario;
    };

    struct Broadcast {};

    struct UploadFile {};

    struct File_AN {
      char accion; // f	
      char tam_remitente[2];
      char *remitente;
    };

    struct Exit {
      char accion; // x
    };

  };

  struct Server {
    struct Login {};

    struct Lista {};

    struct Message {
      char accion; // M
      char tam_msg[3];
      char tam_remitente[2];
      char *msg;
      char *remitente;
    };

    struct Broadcast {};

    struct UploadFile {};

    struct File_AN {
      char accion; // F	
      char tam_usuario[2];
      char *usuario;
    };

    struct Exit {
      char accion; // X
    };

    struct Error {};
  }; 

};
