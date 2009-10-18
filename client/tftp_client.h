#include "../server/main.h"

#ifndef TFTPCLIENT
#define TFTPCLIENT

class TFTPClient {

  private:

    char* server_ip;

    //- kliento socketo descriptorius
    int socket_descriptor;

    //- socket'o endpoint'u strukturos
    struct sockaddr_in client_address;
    int connection;

  protected:

    int sendBuffer(char *);

  public:

  TFTPClient(char* ip);
  ~TFTPClient();

  int connectToServer();

};

class ETFTPSocketCreate: public std::exception {
  virtual const char* what() const throw() {
    return "Unable to create a socket";
  }
};

class ETFTPSocketInitialize: public std::exception {
  virtual const char* what() const throw() {
    return "Unable to find socket library";
  }
};

void debugMessage(char*);

#endif
