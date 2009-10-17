#ifndef TFTPCLIENT
#define TFTPCLIENT

class TFTPClient {

  private:

    char* server_ip;

  protected:

    //- socket'o endpoint'u strukturos
    struct sockaddr_in client_address;

  public:

  TFTPClient(char* ip);

};

#endif