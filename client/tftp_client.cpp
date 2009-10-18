#include "../server/main.h"
#include "tftp_client.h"

using namespace std;

TFTPClient::TFTPClient(char* ip) {

  server_ip = ip;

  //- standartines reiksmes

  socket_descriptor = -1;

  //- wsa

  #ifdef WIN32

  /* edited */

    WSADATA wsaData;

    WORD wVersionRequested = MAKEWORD(2, 2);

    int err = WSAStartup(wVersionRequested, &wsaData);

    if (err != 0) {

        throw new ETFTPSocketInitialize;
        
    }

  #endif

}

int TFTPClient::connectToServer() {

    socket_descriptor = socket(PF_INET, SOCK_STREAM, 0);

    if (socket_descriptor == -1) {

        throw new ETFTPSocketCreate;

    }

    debugMessage("Socket created");

    client_address.sin_family = AF_INET;
    client_address.sin_port = PORT;
    client_address.sin_addr.s_addr = inet_addr(this->server_ip);

    #ifdef WIN32
        //memset(client_address.sin_zero, 0, sizeof(client_address.sinzero);
        //- suvienodinam SOCKADDR_IN ir SOCKADDR strukturu dydzius
    #endif

    connection = connect(socket_descriptor, (const struct sockaddr *)&client_address, sizeof(client_address));

    if (connection != 0) {

        cout << "Unable to connect to an address\n";
        return -1;
        
    }

    debugMessage("Successfully connected");

    sendBuffer("hellow world");

    return 1;

}

int TFTPClient::sendBuffer(char *buffer) {

    return send(socket_descriptor, buffer, (int)strlen(buffer), 0);

}

TFTPClient::~TFTPClient() {

    if (socket_descriptor != -1) {

        #ifdef WIN32

            closesocket(socket_descriptor);
            WSACleanup();
            
        #else

            close(socket_descriptor);

        #endif

    }

}

void debugMessage(char *msg) {
    #ifdef DEBUG
	std::cout << msg << "\n";
    #endif
}